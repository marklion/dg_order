#include "dg_data_opt.h"
#include <time.h>
#include <curl/curl.h>
#include "CJsonObject.hpp"
#include <fstream>
#include <uuid/uuid.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include "Base64.h"
#include <algorithm>

static tdf_log g_log("dg_data_opt");

std::unique_ptr<dg_db_user_info> get_online_user_info(const std::string& _ssid)
{
    auto pret = sqlite_orm::search_record<dg_db_user_login>(DG_DB_FILE, "ssid = '%s'", _ssid.c_str());
    std::unique_ptr<dg_db_user_info> p_user_info;
    if (pret)
    {
        auto cur_time = time(NULL) / 3600;
        if (cur_time < pret->m_login_timestamp_hour || (cur_time - pret->m_login_timestamp_hour) > 12)
        {
            pret->remove_record();
            pret.reset();
        }
        else
        {
            pret->m_login_timestamp_hour = cur_time;
            pret->update_record();
            p_user_info.reset( sqlite_orm::search_record<dg_db_user_info>(DG_DB_FILE, pret->m_user_info_id).release()) ;
        }
    }

    return p_user_info;
}

static size_t dg_proc_curl(void *ptr, size_t size, size_t nmemb, void *user_data)
{
    auto in_buff = (std::string *)user_data;
    in_buff->append((char *)ptr, size * nmemb);

    return size * nmemb;
}

static std::string dg_rest_post(const std::string &_url, const std::string &_json)
{
    std::string in_buff;
    auto curlhandle = curl_easy_init();
    if (nullptr != curlhandle)
    {
        curl_easy_setopt(curlhandle, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, &in_buff);
        curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, dg_proc_curl);
        // 设置post提交方式
        curl_easy_setopt(curlhandle, CURLOPT_POST, 1);
        // 设置post的数据
        curl_easy_setopt(curlhandle, CURLOPT_POSTFIELDS, _json.c_str());
        curl_easy_perform(curlhandle);
        curl_easy_cleanup(curlhandle);
    }

    return in_buff;
}

static std::string dg_rest_req(const std::string &_req)
{
    std::string in_buff;
    auto curlhandle = curl_easy_init();
    if (nullptr != curlhandle)
    {
        curl_easy_setopt(curlhandle, CURLOPT_URL, _req.c_str());
        curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, &in_buff);
        curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, dg_proc_curl);
        curl_easy_perform(curlhandle);
        curl_easy_cleanup(curlhandle);
    }

    return in_buff;
}
static std::string dg_store_logo_to_file(const std::string &_logo, const std::string &_upid)
{
    std::string ret;
    std::string file_name("/dist/logo_res/logo_");
    file_name.append(_upid);
    file_name.append(".jpg");
    
    std::fstream out_file;
    out_file.open(file_name.c_str(), std::ios::binary|std::ios::out|std::ios::trunc);
    if (out_file.is_open())
    {
        out_file.write(_logo.data(), _logo.length());
        ret = file_name;
        ret.erase(ret.begin(), ret.begin() + 5);
        out_file.close();
    }
    else
    {
        g_log.err("logo store file openned failed");
    }
    

    return ret;
}

static std::string dg_store_good_picture(const std::string &_pic, const std::string &_name)
{
    std::string ret;
    std::string file_name("/dist/logo_res/good_pic_");

    std::string base_name;
    Base64::Encode(_name, &base_name);
    std::replace(base_name.begin(), base_name.end(), '/', '_');
    std::replace(base_name.begin(), base_name.end(), '+', '-');

    file_name.append(base_name);
    file_name.append(std::to_string(time(NULL)));
    file_name.append(".jpg");
    
    std::fstream out_file;
    out_file.open(file_name.c_str(), std::ios::binary|std::ios::out|std::ios::trunc);
    if (out_file.is_open())
    {
        out_file.write(_pic.data(), _pic.length());
        ret = file_name;
        ret.erase(ret.begin(), ret.begin() + 5);
        out_file.close();
    }
    else
    {
        g_log.err("pic store file openned failed");
    }

    return ret;

}

static std::string dg_gen_ssid()
{
    uuid_t out;
    std::string ret;

    uuid_generate(out);
    char byte_show[3];
    for (auto itr : out)
    {
        sprintf(byte_show, "%02X", itr);
        ret.append(byte_show);
    }

    return ret;
}
static std::unique_ptr<dg_db_user_info> fetch_user_info(const std::string &_name, const std::string &_logo, const std::string &_openid)
{
    auto p_user_info = sqlite_orm::search_record<dg_db_user_info>(DG_DB_FILE, "openid = '%s'", _openid.c_str());
    if (nullptr == p_user_info)
    {
        p_user_info.reset(new dg_db_user_info(DG_DB_FILE));
        p_user_info->m_openid = _openid;
        p_user_info->insert_record();
    }
    p_user_info->m_name = _name;
    p_user_info->m_logo = _logo;
    
    p_user_info->update_record();

    return p_user_info;
}
static dg_db_user_login dg_pull_user_info_from_wechat(const std::string& _acctok, const std::string &_open_id)
{
    dg_db_user_login ret(DG_DB_FILE);
    std::string req = "https://api.weixin.qq.com/sns/userinfo?access_token=" + _acctok + "&openid=" + _open_id + "&lang=zh_CN";
    auto in_buff = dg_rest_req(req);

    g_log.log("user infor:" + in_buff);
    neb::CJsonObject oJson(in_buff);

    if (oJson.KeyExist("errcode"))
    {
        g_log.err(oJson("errmsg"));
    }
    else
    {
        auto logo_path = oJson("headimgurl");
        auto logo_content = dg_rest_req(logo_path);
        auto p_user_info = fetch_user_info(oJson("nickname"), dg_store_logo_to_file(logo_content, _open_id), _open_id);
        if (p_user_info)
        {
            ret.m_ssid = dg_gen_ssid();
            ret.m_user_info_id = p_user_info->get_pri_id();
            ret.m_login_timestamp_hour = time(NULL) / 3600;
        }
    }

    return ret;
}
std::string dg_wechat_login(const std::string& _code)
{
    std::string ret;
    std::string wechat_secret(getenv("WECHAT_SECRET"));
    std::string req = "https://api.weixin.qq.com/sns/oauth2/access_token?appid=wxa390f8b6f68e9c6d&secret=" + wechat_secret + "&code=" + _code + "&grant_type=authorization_code";

    std::string in_buff = dg_rest_req(req);
    neb::CJsonObject oJson(in_buff);

    if (oJson.KeyExist("errcode"))
    {
        g_log.err("failed to get openid: %s", oJson("errmsg"));
    }
    else
    {
        // search if user already login, if so, return ssid
        auto p_user_info = get_user_info(oJson("openid"));
        if (p_user_info)
        {
            auto pexist_user = sqlite_orm::search_record<dg_db_user_login>(DG_DB_FILE, "user_info_id = %d", p_user_info->get_pri_id());
            if (pexist_user)
            {
                // call this below function to confirm if login timeout
                auto search_user = get_online_user_info(pexist_user->m_ssid);
                if (search_user)
                {
                    ret = pexist_user->m_ssid;
                }
            }
        }
        if (ret.length() <= 0)
        {
            auto user_information = dg_pull_user_info_from_wechat(oJson("access_token"), oJson("openid"));
            if (true == user_information.insert_record())
            {
                ret = user_information.m_ssid;
            }
        }
    }

    return ret;
}

std::unique_ptr<dg_db_user_info> get_user_info(const std::string& _openid)
{
    return sqlite_orm::search_record<dg_db_user_info>(DG_DB_FILE, "openid = '%s'", _openid.c_str());
}
std::unique_ptr<dg_db_user_info> get_user_info(int _user_info_id)
{
    return sqlite_orm::search_record<dg_db_user_info>(DG_DB_FILE, _user_info_id);
}


std::string dg_create_order(int _user_id, const std::string& _destination, const std::string& _start_time, const std::string& _deliver_time, const std::string& _comments, const std::string &_contact_qr)
{
    dg_db_order_owner db_record(DG_DB_FILE);
    std::string ret;

    db_record.m_owner_user_id= _user_id;
    db_record.m_destination = _destination;
    db_record.m_start_time = _start_time;
    db_record.m_deliver_time = _deliver_time;
    db_record.m_comments = _comments;
    db_record.m_contact_qr = get_contact_qr_from_wx(_contact_qr);

    if (db_record.insert_record())
    {
        ret = std::to_string(db_record.get_pri_id());
    }

    return ret;
}

std::unique_ptr<dg_db_order_owner> dg_get_order(const std::string& _order_id)
{
    return  sqlite_orm::search_record<dg_db_order_owner>(DG_DB_FILE, "PRI_ID = %s", _order_id.c_str());
}

void dg_get_goods_name_by_order_id(const std::string& _order_id, std::function<bool ( const dg_db_goods &)> const &f)
{
    auto p_goods_by_name = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = '%s' group by good_id", _order_id.c_str());
    for (auto &itr: p_goods_by_name)
    {
        if (false == f(itr))
        {
            break;
        }
    }
}
void dg_get_buyer_by_name(const std::string &_order_id, int _good_id, std::function<bool ( const dg_db_goods &)> const &f)
{
    auto p_goods_buyers = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = '%s' AND good_id = %d GROUP BY user_id,spec", _order_id.c_str(), _good_id);
    for (auto &itr:p_goods_buyers)
    {
        if  (false == f(itr))
        {
            break;
        }
    }
}

int dg_get_count_by_buyer_spec(const std::string &_order_id, int _good_id, int _user_id, const std::string &_spec)
{
    auto p_result = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = '%s' AND good_id = %d AND user_id = %d AND spec = '%s'",
                                                               _order_id.c_str(), _good_id, _user_id, _spec.c_str());

    return p_result.size();
}

struct content_from_wx {
    std::string m_content_from_wx;
    long m_expires_timestamp = 0;
    virtual void refresh_content() = 0;
    bool is_expired() {
        bool ret = false;
        if (time(nullptr) > m_expires_timestamp)
        {
            ret = true;
        }

        return ret;
    }
    std::string get_content() {
        std::string ret;
        if (is_expired())
        {
            refresh_content();
            ret = m_content_from_wx;
        }
        else
        {
            ret = m_content_from_wx;
            g_log.log("content still ready");
        }
        
        return ret;
    }
};
struct acc_tok_from_wx:content_from_wx {
    void refresh_content() {
        std::string wechat_secret(getenv("WECHAT_SECRET"));
        auto in_buff = dg_rest_req("https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=wxa390f8b6f68e9c6d&secret=" + wechat_secret);

        g_log.log("recv tok from wx:%s", in_buff.c_str());
        neb::CJsonObject oJson(in_buff);

        if (oJson.KeyExist("errcode"))
        {
            g_log.err("failed when req acc_tok:%s", oJson("errmsg"));
        }
        else
        {
            m_content_from_wx = oJson("access_token");
            m_expires_timestamp = time(NULL) + atoi(oJson("expires_in").c_str());
        }
        

    }
} g_acc_tok;

struct jsapi_ticket_from_wx:content_from_wx {
    void refresh_content() {
        auto in_buff = dg_rest_req("https://api.weixin.qq.com/cgi-bin/ticket/getticket?access_token=" + g_acc_tok.get_content() + "&type=jsapi");

        g_log.log("recv js_ticket from wx:%s", in_buff.c_str());
        neb::CJsonObject oJson(in_buff);

        if ("0" != oJson("errcode"))
        {
            g_log.err("failed when req js_ticket:%s", oJson("errmsg"));
        }
        else
        {
            m_content_from_wx = oJson("ticket");
            m_expires_timestamp = time(NULL) + atoi(oJson("expires_in").c_str());
        }

    }
} g_jsapi_ticket;

static int dg_fetch_good_info(const std::string &_name, const std::string &_picture)
{
    int ret = -1;

    auto good_info = sqlite_orm::search_record<dg_db_good_info>(DG_DB_FILE, "name = '%s'", _name.c_str());
    if (good_info)
    {
        if (good_info->m_picture.length() <= 0)
        {
            if (_picture.length() > 0 && _picture != "none")
            {
                auto downlaod_img = dg_rest_req("https://api.weixin.qq.com/cgi-bin/media/get?access_token=" 
                    + g_acc_tok.get_content() + "&media_id=" + _picture);
                good_info->m_picture = dg_store_good_picture(downlaod_img, _name);
                good_info->update_record();
            }
        }
        ret = good_info->get_pri_id();
    }
    else
    {
        dg_db_good_info new_good(DG_DB_FILE);
        new_good.m_name = _name;
        if (_picture.length() > 0 && _picture != "none")
        {
            auto downlaod_img = dg_rest_req("https://api.weixin.qq.com/cgi-bin/media/get?access_token=" + g_acc_tok.get_content() + "&media_id=" + _picture);
            new_good.m_picture = dg_store_good_picture(downlaod_img, _name);
        }
        new_good.insert_record();
        ret = new_good.get_pri_id();
    }

    return ret;
}

std::unique_ptr<dg_db_good_info> dg_get_good_info(int _good_id)
{
    return sqlite_orm::search_record<dg_db_good_info>(DG_DB_FILE, _good_id);
}

std::string dg_insert_goods(const std::string &_good_name, const std::string &_spec, const std::string &_picture, int _order_id, int _user_id)
{
    std::string ret;

    dg_db_goods good(DG_DB_FILE);

    good.m_good_id = dg_fetch_good_info(_good_name, _picture);
    good.m_order_id = _order_id;
    good.m_user_id = _user_id;
    good.m_spec = _spec;

    auto goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %d AND user_id = %d AND good_id = %d AND spec = '%s'", _order_id, _user_id, good.m_good_id, _spec.c_str());
    for (auto &itr:goods)
    {
        if (itr.m_price.length() > 0)
        {
            good.m_price = itr.m_price;
            good.m_pending = itr.m_pending;
            break;
        }
    }

    if (good.insert_record())
    {
        ret = std::to_string(good.get_pri_id());
    }

    return ret;
}

const std::string dg_wx_sign(const std::string& nonceStr, long timestamp, const std::string &url)
{
    auto jsapi_ticket = g_jsapi_ticket.get_content();
    std::string s1 = "jsapi_ticket=" + jsapi_ticket;
    s1.append("&noncestr=" + nonceStr);
    s1.append("&timestamp=" + std::to_string(timestamp));
    s1.append("&url=" + url);

    SHA_CTX c;
    unsigned char md[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)s1.c_str(), s1.length(), md);
    SHA1_Init(&c);
    SHA1_Update(&c, s1.c_str(), s1.length());
    SHA1_Final(md, &c);
    OPENSSL_cleanse(&c, sizeof(c));

    std::string ret;

    for (size_t i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        char tmp[3] = {0};
        sprintf(tmp, "%02x", md[i]);
        ret.append(tmp);
    }
    
    return ret;
}

void dg_get_self_good_by_order_id(const std::string &_order_id, int _user_id, std::function<bool ( const dg_db_goods &)> const &f)
{
    auto self_goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %s AND user_id = %d", _order_id.c_str(), _user_id);
    for (auto &itr:self_goods)
    {
        if (false == f(itr))
        {
            break;
        }
    }
}

void dg_get_joined_id(int _user_id, std::function<bool ( const dg_db_goods &)> const &f)
{
    auto orders = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "user_id = %d GROUP BY order_id", _user_id);
    for (auto &itr:orders)
    {
        if (false == f(itr))
        {
            break;
        }
    }
}

std::unique_ptr<dg_db_goods> dg_get_order_good(int _order_id, int _user_id, const std::string &_name, const std::string &_spec)
{
    return sqlite_orm::search_record<dg_db_goods>(DG_DB_FILE, "order_id = %d AND user_id = %d AND name = '%s' AND spec = '%s'", _order_id, _user_id, _name.c_str(), _spec.c_str());
}

void dg_get_self_good_by_order_id_and_name_and_spec(const std::string &_order_id, int _user_id, const std::string &_name, const std::string &_spec, std::function<bool (dg_db_goods &)> const &f)
{
    auto good_id = dg_fetch_good_info(_name, "");
    auto goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %s AND user_id = %d AND good_id = %d AND spec = '%s'", _order_id.c_str(), _user_id, good_id, _spec.c_str());
    for (auto &itr:goods) {
        if (false == f(itr))
        {
            break;
        }
    }
}

std::unique_ptr<dg_db_goods> dg_get_order_good(int _priv_id)
{
    return sqlite_orm::search_record<dg_db_goods>(DG_DB_FILE, "PRI_ID = %d", _priv_id);
}

static std::string make_cur_time_string()
{
    auto sec = time(NULL);
    struct tm lt;

    localtime_r(&sec, &lt);
    std::string ret;

    ret = std::to_string(lt.tm_year + 1900) + "-" + std::to_string(lt.tm_mon + 1) + "-" + std::to_string(lt.tm_mday);

    g_log.log("excute at %s", ret.c_str());

    return ret;
}

void send_out_sub_msg(int _my_good_id, const std::string &_touser,  const std::string &_name, const std::string &_status, const std::string &_remark, const std::string &_url )
{
    neb::CJsonObject msg_json;

    msg_json.Add("touser", _touser);
    msg_json.Add("template_id", "IIQyzBY8hAScYSFPmGQ1bjbVyOtlApgTsTGWtMZDBHs");
    if (_url.length() == 0)
    {
        msg_json.Add("url", std::string("http://www.d8sis.cn/my_goods/") + std::to_string(_my_good_id));
    }
    else
    {
        msg_json.Add("url", _url);
    }
    
    
    neb::CJsonObject msg_data;
    neb::CJsonObject data_value;
    data_value.Add("value", "");

    auto order_brief = dg_get_order(std::to_string(_my_good_id));
    auto dg_owner = get_user_info(order_brief->m_owner_user_id);
    if (order_brief && dg_owner)
    {
        std::string order_description = dg_owner->m_name + "--" + order_brief->m_destination + "代购";
        std::string title = "您好，您在 " + order_description + " 中的 " + _name + " 状态已更新";
        data_value.Replace("value", title);
        msg_data.Add("first", data_value);
    }
    else
    {
        return;
    }
    

    std::map<std::string, std::string> status_ch_map;
    status_ch_map["booking"] = "预定中";
    status_ch_map["bought"] = "已购买";
    status_ch_map["delivered"] = "已发货";

    data_value.Replace("value", status_ch_map[_status]);
    msg_data.Add("keyword1", data_value);

    data_value.Replace("value", make_cur_time_string());
    msg_data.Add("keyword2", data_value);

    if (_remark.length() == 0)
    {
        data_value.Replace("value", "更多信息请查看订单详情");
    }
    else
    {
        data_value.Replace("value", _remark);
    }
    
    msg_data.Add("remark", data_value);

    msg_json.Add("data", msg_data);

    auto post_ret = dg_rest_post(std::string("https://api.weixin.qq.com/cgi-bin/message/template/send?access_token=") + g_acc_tok.get_content(), msg_json.ToString());
    g_log.log("send sub post:%s to wx and get resutl:%s", msg_json.ToString().c_str(), post_ret.c_str());
}

bool dg_get_sub_status_from_wx(const std::string &_ssid)
{
    bool ret = false;

    auto puser = get_online_user_info(_ssid);
    if (puser)
    {
        auto result = dg_rest_req("https://api.weixin.qq.com/cgi-bin/user/info?access_token=" + g_acc_tok.get_content() + "&openid=" + puser->m_openid + "&lang=zh_CN");
        neb::CJsonObject res_json(result);
        if (res_json.KeyExist("errcode"))
        {
            g_log.err("failed to get user info from wechat:%s", res_json("errmsg").c_str());
        }
        else
        {
            int sub_status;
            res_json.Get("subscribe", sub_status);
            if (0 != sub_status)
            {
                ret = true;
            }
        }
        
    }

    return ret;
}

std::string get_contact_qr_from_wx(const std::string& _contact_qr)
{
    auto downlaod_img = dg_rest_req("https://api.weixin.qq.com/cgi-bin/media/get?access_token=" + g_acc_tok.get_content() + "&media_id=" + _contact_qr);
    return dg_store_good_picture(downlaod_img, dg_gen_ssid());
}