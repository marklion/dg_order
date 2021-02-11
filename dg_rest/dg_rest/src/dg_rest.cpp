// This file generated by ngrestcg
// For more information, please visit: https://github.com/loentar/ngrest

#include "dg_rest.h"
#include <stdlib.h>
#include "Base64.h"
#include "../../../public.h"
#include "../../../dg_data_base/dg_data_opt.h"
#include <map>
#include <thread>

static tdf_log g_log("dg_rest");

void dg_rest::proc_wechat_login_async(const std::string& code, ngrest::Callback<const std::string&>& callback)
{
    g_log.log("recv rest req: wechat_login, code: %s", code.c_str());

    std::thread([&callback, code]{
        auto ret = dg_wechat_login(code);
        ngrest::Handler::post([&callback, ret]{
            callback.success(ret);
        });
    }).detach();
    
}


user_info dg_rest::proc_get_user_info(const std::string& ssid)
{
    user_info ret = {false, "", ""};
    g_log.log("recv rest req: user_info, ssid: %s", ssid.c_str());

    auto p_user_info = get_online_user_info(ssid);
    if (p_user_info)
    {
        ret.online = true;
        Base64::Encode(p_user_info->m_name, &(ret.user_name));
        ret.user_logo = p_user_info->m_logo;
    }

    return ret;
}

static std::string show_dg_order_config(const dg_order_config &_order)
{
    std::string ret;
    std::string dest;
    Base64::Decode(_order.info.destination, &dest);
    ret.append("destination:" + dest + "-----" + "start time:" + _order.info.start_time + "-----" + _order.ssid);

    return ret;
}

std::string dg_rest::proc_dg_order_create(const dg_order_config& order)
{
    g_log.log("recv rest req: dg_order post: %s", show_dg_order_config(order).c_str());
    std::string ret;

    auto p_onlien_user = get_online_user_info(order.ssid);
    if (p_onlien_user)
    {
        std::string dest;
        Base64::Decode(order.info.destination, &dest);
        std::string comments;
        Base64::Decode(order.info.comments, &comments);
        ret = dg_create_order(p_onlien_user->get_pri_id(), dest, order.info.start_time, order.info.deliver_time, comments, order.info.contact_qr);
    }

    return ret;
}


dg_order_show dg_rest::proc_dg_get_order(const std::string& order_id)
{
    g_log.log("recv rest req: dg_order get id: %s", order_id.c_str());
    dg_order_show ret;

    auto p_order_conf = dg_get_order(order_id);
    if (p_order_conf)
    {
        auto p_order_owner = get_user_info(p_order_conf->m_owner_user_id);
        if (p_order_owner)
        {
            Base64::Encode(p_order_owner->m_name, &(ret.order_owner_name));
            ret.order_owner_logo = p_order_owner->m_logo;

            Base64::Encode(p_order_conf->m_destination, &(ret.info.destination));
            Base64::Encode(p_order_conf->m_comments, &(ret.info.comments));
            ret.info.deliver_time = p_order_conf->m_deliver_time;
            ret.info.start_time = p_order_conf->m_start_time;
            ret.info.contact_qr = p_order_conf->m_contact_qr;
            ret.order_id = p_order_conf->get_pri_id();
        }
    }

    return ret;
}

std::vector<dg_goods_show> dg_rest::proc_dg_get_goods(const std::string& order_id)
{
    std::vector<dg_goods_show> ret;

    dg_get_goods_name_by_order_id(order_id, [&](const dg_db_goods & _good)->bool {
        auto good_info = dg_get_good_info(_good.m_good_id);
        if (good_info == nullptr)
        {
            return false;
        }
        dg_goods_show tmp;
        Base64::Encode(good_info->m_name, &(tmp.name));
        tmp.picture = good_info->m_picture;
        tmp.total = 0;
        dg_get_buyer_by_name(order_id, _good.m_good_id, [&](const dg_db_goods & _buyer_good)->bool {
            dg_goods_buyer_info tmp_buyer;
            auto p_buyer_user = get_user_info(_buyer_good.m_user_id);
            if (p_buyer_user)
            {
                Base64::Encode(p_buyer_user->m_name, &(tmp_buyer.user_name));
                tmp_buyer.user_logo = p_buyer_user->m_logo;
                Base64::Encode(_buyer_good.m_spec, &(tmp_buyer.spec));
                tmp_buyer.number = dg_get_count_by_buyer_spec(order_id, _good.m_good_id, _buyer_good.m_user_id, _buyer_good.m_spec);
                tmp.total += tmp_buyer.number;
                tmp.buyer.push_back(tmp_buyer);
            }

            return true;
        });
        ret.push_back(tmp);
        return true;
    });

    return ret;
}

std::string dg_rest::proc_dg_insert_goods(const dg_good_insert& good)
{
    std::string ret = "failed";
    auto p_online_user = get_online_user_info(good.ssid);
    if (p_online_user)
    {
        ret = "success";
        for (int i = 0; i < good.number; i++)
        {
            std::string my_name;
            Base64::Decode(good.name, &my_name);
            std::string my_spec;
            Base64::Decode(good.spec, &my_spec);
            if (my_spec.length() <= 0)
            {
                my_spec = "默认规格";
            }
            if (dg_insert_goods(my_name, my_spec, good.img, atoi(good.order_id.c_str()), p_online_user->get_pri_id()).size() <= 0)
            {
                ret = "failed";
                break;
            }
        }
    }

    return ret;
}

std::string dg_rest::proc_wx_sign(const std::string& nonceStr, long timestamp, const std::string& url)
{
    return dg_wx_sign(nonceStr, timestamp, url);
}

std::vector<dg_self_good> dg_rest::proc_my_order_get(const std::string& order_id, const std::string& ssid)
{
    std::vector<dg_self_good> ret;
    auto opt_user = get_online_user_info(ssid);
    if (opt_user)
    {
        std::map<std::string, dg_self_good *> tmp_ret;
        dg_get_self_good_by_order_id(order_id, opt_user->get_pri_id(), [&](const dg_db_goods &_good) -> bool {
            auto good_info = dg_get_good_info(_good.m_good_id);
            if (good_info == nullptr)
            {
                return false;
            }
            auto charect = good_info->m_name + _good.m_spec + _good.m_status;
            auto p_exist = tmp_ret[charect];
            if (nullptr == p_exist)
            {
                tmp_ret[charect] = new dg_self_good();
                p_exist = tmp_ret[charect];
                Base64::Encode(good_info->m_name, &(p_exist->name));
                Base64::Encode(_good.m_spec, &(p_exist->spec));
                p_exist->picture = good_info->m_picture;
                p_exist->number = 0;
                p_exist->status = _good.m_status;
                Base64::Encode(_good.m_address, &(p_exist->address));
                p_exist->express = _good.m_express;
                p_exist->pending = _good.m_pending;
                p_exist->price = 0;
            }
            p_exist->number++;
            if (_good.m_price.length() > 0)
            {
                p_exist->price += atoi(_good.m_price.c_str());
            }

            return true;
        });
        for (auto itr = tmp_ret.begin(); itr != tmp_ret.end(); itr++)
        {
            ret.push_back(*(itr->second));
            delete itr->second;
        }
    }

    return ret;
}

std::vector<std::string> dg_rest::proc_my_orders(const std::string ssid)
{
    std::vector<std::string> ret;
    auto opt_user = get_online_user_info(ssid);

    if (opt_user)
    {
        dg_get_joined_id(opt_user->get_pri_id(), [&](const dg_db_goods &_good)->bool {
            ret.push_back(std::to_string(_good.m_order_id));
            return true;
        });
    }

    return ret;
}

std::string dg_rest::proc_update_order(const dg_update_order_good& update_info)
{
    std::string ret = "failed";
    int total_count = update_info.content.number;

    auto opt_user = get_online_user_info(update_info.ssid);
    if (opt_user)
    {
        std::string my_name;
        std::string my_spec;
        Base64::Decode(update_info.name, &my_name);
        Base64::Decode(update_info.spec, &my_spec);
        dg_get_self_good_by_order_id_and_name_and_spec(update_info.order_id, opt_user->get_pri_id(), my_name, my_spec, [&](dg_db_goods &_good) -> bool {
            if (total_count > 0)
            {
                std::string con_spec;
                Base64::Decode(update_info.content.spec, &con_spec);
                if (con_spec.length() <= 0)
                {
                    con_spec = "默认规格";
                }
                if (con_spec != _good.m_spec)
                {
                    _good.m_spec = con_spec;
                    _good.update_record();
                }
            }
            else
            {
                _good.remove_record();                
            }
            total_count--;
            return true;
        });
        for (int i = 0; i < total_count; i++)
        {
            std::string con_spec;
            Base64::Decode(update_info.content.spec, &con_spec);
            dg_insert_goods(my_name, con_spec, "none", atoi(update_info.order_id.c_str()), opt_user->get_pri_id());
        }
        ret = "success";
    }

    return ret;
}

std::string dg_rest::proc_delete_order(const dg_delete_order_good& delete_info)
{
    std::string my_name;
    std::string my_spec;
    Base64::Decode(delete_info.name, &my_name);
    Base64::Decode(delete_info.spec, &my_spec);

    auto opt_user = get_online_user_info(delete_info.ssid);
    if (opt_user)
    {
        dg_get_self_good_by_order_id_and_name_and_spec(delete_info.order_id, opt_user->get_pri_id(), my_name, my_spec, [&](dg_db_goods &_good) -> bool {
            _good.remove_record();
            return true;
        });
    }

    return "success";
}

std::vector<dg_order_created_resp> dg_rest::proc_created_orders(const std::string ssid)
{
    std::vector<dg_order_created_resp> ret;

    auto user = get_online_user_info(ssid);
    if (user)
    {
        auto orders = sqlite_orm::search_record_all<dg_db_order_owner>(DG_DB_FILE, "owner_user_id = %d", user->get_pri_id());
        for (auto &itr:orders)
        {
            dg_order_created_resp tmp;
            tmp.id = itr.get_pri_id();
            Base64::Encode(user->m_name, &(tmp.user_name));
            tmp.user_logo = user->m_logo;
            Base64::Encode(itr.m_destination, &(tmp.destination));
            tmp.start_time = itr.m_start_time;
            tmp.deliver_time = itr.m_deliver_time;
            Base64::Encode(itr.m_comments, &(tmp.comments));
            ret.push_back(tmp);
        }
    }
    return ret;
}


std::vector<std::string> dg_rest::proc_get_specs_by_name(const std::string& good_name)
{
    std::vector<std::string> ret;
    std::string my_name;
    g_log.log("good_name is %s", good_name.c_str());

    Base64::Decode(good_name, &my_name, true);
    auto good = sqlite_orm::search_record<dg_db_good_info>(DG_DB_FILE, "name = '%s'", my_name.c_str());

    if (good) {
        auto specs = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "good_id = %d GROUP BY spec", good->get_pri_id());
        for (auto &itr:specs)
        {
            std::string my_spec;
            Base64::Encode(itr.m_spec, &my_spec);
            ret.push_back(my_spec);
        }
    }

    return ret;
}

std::string dg_rest::proc_order_brief_change(const std::string& ssid, const dg_order_brief_change& order_brief)
{
    std::string ret = "failed";

    auto opt_user = get_online_user_info(ssid);
    if (opt_user)
    {
        auto order_info = dg_get_order(std::to_string(order_brief.id));
        if (order_info && order_info->m_owner_user_id == opt_user->get_pri_id())
        {
            Base64::Decode(order_brief.comments, &(order_info->m_comments));
            Base64::Decode(order_brief.destination, &(order_info->m_destination));
            order_info->m_start_time = order_brief.start_time;
            order_info->m_deliver_time = order_brief.deliver_time;
            order_info->m_contact_qr = get_contact_qr_from_wx(order_brief.contact_qr);
            if (order_info->update_record())
            {
                ret = "success";
            }
        }
    }

    return ret;
}

bool dg_rest::proc_host_of(const std::string& ssid, const std::string& order_id)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    if (opt_user)
    {
        auto order = dg_get_order(order_id);
        if (order && order->m_owner_user_id == opt_user->get_pri_id())
        {
            ret = true;
        }
    }

    return ret;
}

std::vector<dg_all_goods_order> dg_rest::proc_all_goods(const std::string& order_id)
{
    std::vector<dg_all_goods_order> ret;

    auto all_goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = '%s'", order_id.c_str());
    for (auto &single_good:all_goods)
    {
        auto good_info = dg_get_good_info(single_good.m_good_id);
        auto user_info = get_user_info(single_good.m_user_id);
        if (good_info != nullptr && nullptr != user_info)
        {
            dg_all_goods_order order;
            order.id = single_good.get_pri_id();
            Base64::Encode(user_info->m_name, &(order.user_name));
            order.user_logo = user_info->m_logo;
            Base64::Encode(good_info->m_name, &(order.name));
            Base64::Encode(single_good.m_spec, &(order.spec));
            order.picture = good_info->m_picture;
            if (single_good.m_status.length() <= 0)
            {
                single_good.m_status = "booking";
                single_good.update_record();
            }
            order.status = single_good.m_status;
            Base64::Encode(single_good.m_address, &(order.address));
            order.express = single_good.m_express;
            order.price = single_good.m_price;
            order.pending = single_good.m_pending;
            ret.push_back(order);
        }
    }

    return ret;
}



bool dg_rest::proc_update_status(const std::string& ssid, int id, const std::string& status)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    if (opt_user)
    {
        auto good_record = dg_get_order_good(id);
        if (good_record)
        {
            auto order = dg_get_order(std::to_string( good_record->m_order_id));
            if (order && order->m_owner_user_id == opt_user->get_pri_id())
            {
                if (good_record->m_status != status)
                {
                    good_record->m_status = status;
                    if (good_record->m_status != "delivered")
                    {
                        good_record->m_express = "";
                    }
                    ret = good_record->update_record();
                    if (true == ret)
                    {
                        auto to_user = get_user_info(good_record->m_user_id);
                        auto good_info = dg_get_good_info(good_record->m_good_id);
                        send_out_sub_msg(order->get_pri_id(), to_user->m_openid, good_info->m_name, status, "");
                    }
                }
            }
        }
    }

    return ret;
}

bool dg_rest::proc_update_address(const std::string& ssid, const std::string& order_id, const std::string& name, const std::string& spec, const std::string& address)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    std::string my_name;
    std::string my_spec;
    
    Base64::Decode(name, &my_name);
    Base64::Decode(spec, &my_spec);

    auto good_info = sqlite_orm::search_record<dg_db_good_info>(DG_DB_FILE, "name = '%s'", my_name.c_str());
    if (opt_user && good_info)
    {
        auto goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %s AND user_id = %d AND good_id = %d AND spec = '%s'", order_id.c_str(), opt_user->get_pri_id(), good_info->get_pri_id(), my_spec.c_str());
        for (auto &itr:goods)
        {
            Base64::Decode(address, &(itr.m_address));
            ret = itr.update_record();
        }
    }

    return ret;
}

bool dg_rest::proc_update_express(const std::string& ssid, int id, const std::string& express)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    auto good_record = dg_get_order_good(id);
    if (opt_user && good_record)
    {
        auto order_brief = dg_get_order(std::to_string(good_record->m_order_id));
        if (order_brief && opt_user->get_pri_id() == order_brief->m_owner_user_id)
        {
            auto likely_goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %d AND good_id = %d AND spec = '%s' AND status = '%s' AND express = '%s'",
                                                        good_record->m_order_id, good_record->m_good_id, good_record->m_spec.c_str(), good_record->m_status.c_str(), good_record->m_express.c_str());
            for (auto &itr:likely_goods)
            {
                itr.m_express = express;
                ret = itr.update_record();
                proc_update_status(ssid, id, "delivered");
            }
        }
    }

    return ret;
}

void dg_rest::proc_get_sub_status(const std::string& ssid, ngrest::Callback<bool>& callback)
{
    std::thread([&callback, ssid]{
        auto ret = dg_get_sub_status_from_wx(ssid);
        ngrest::Handler::post([&callback, ret]{
            callback.success(ret);
        });
    }).detach();
}

bool dg_rest::proc_update_pending(const std::string& ssid, int id, const std::string& pending)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    auto order = dg_get_order_good(id);
    if (order)
    {
        auto order_brief = dg_get_order(std::to_string( order->m_order_id));
        if (opt_user && order_brief && opt_user->get_pri_id() == order_brief->m_owner_user_id)
        {
            auto good_info = dg_get_good_info(order->m_good_id);
            auto goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %d AND user_id = %d AND good_id = %d AND spec = '%s'", order->m_order_id, order->m_user_id, good_info->get_pri_id(), order->m_spec.c_str());
            for (auto &itr:goods)
            {
                itr.m_pending = pending;
                ret = itr.update_record();
            }
        }
    }

    return ret;
}

bool dg_rest::proc_update_price(const std::string& ssid, int id, const std::string& price)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    auto order = dg_get_order_good(id);
    if (order)
    {
        auto order_brief = dg_get_order(std::to_string( order->m_order_id));
        if (opt_user && order_brief && opt_user->get_pri_id() == order_brief->m_owner_user_id)
        {
            auto good_info = dg_get_good_info(order->m_good_id);
            auto goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %d AND user_id = %d AND good_id = %d AND spec = '%s'", order->m_order_id, order->m_user_id, good_info->get_pri_id(), order->m_spec.c_str());
            int total_price = 0;
            for (auto &itr : goods)
            {
                itr.m_price= price;
                ret = itr.update_record();
                total_price += atoi(price.c_str());
            }
            if (order->m_price != price)
            {
                std::string remark = "代购主设定了单价：" + price + "元, 总价：" + std::to_string(total_price) + "元  请点击详情确认";
                send_out_sub_msg(order_brief->get_pri_id(), get_user_info(order->m_user_id)->m_openid, dg_get_good_info(order->m_good_id)->m_name + "(" + order->m_spec +  ")", order->m_status, remark);
            }
        }
    }

    return ret;

}

bool dg_rest::proc_confirm_pending(const std::string& ssid, const std::string& order_id, const std::string& name, const std::string& spec, const std::string& pending)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    std::string my_name;
    std::string my_spec;
    
    Base64::Decode(name, &my_name);
    Base64::Decode(spec, &my_spec);

    auto good_info = sqlite_orm::search_record<dg_db_good_info>(DG_DB_FILE, "name = '%s'", my_name.c_str());
    if (opt_user && good_info)
    {
        auto goods = sqlite_orm::search_record_all<dg_db_goods>(DG_DB_FILE, "order_id = %s AND user_id = %d AND good_id = %d AND spec = '%s'", order_id.c_str(), opt_user->get_pri_id(), good_info->get_pri_id(), my_spec.c_str());
        for (auto &itr:goods)
        {
            itr.m_pending = pending;
            ret = itr.update_record();
            std::string remark = opt_user->m_name + "确认了价格";
            send_out_sub_msg(atoi(order_id.c_str()), get_user_info(itr.m_user_id)->m_openid, dg_get_good_info(itr.m_good_id)->m_name + "(" + itr.m_spec +  ")", itr.m_status, remark, std::string("http://www.d8sis.cn/list/") + order_id);
        }
    }

    return ret;
}

bool dg_rest::proc_giveup_good(const std::string& ssid, int id, const std::string& comment)
{
    bool ret = false;

    auto opt_user = get_online_user_info(ssid);
    auto order = dg_get_order_good(id);
    if (order)
    {
        auto order_brief = dg_get_order(std::to_string(order->m_order_id));
        if (opt_user && order_brief && order_brief->m_owner_user_id == opt_user->get_pri_id())
        {
            order->remove_record();
            auto to_user = get_user_info(order->m_user_id);
            auto good_info = dg_get_good_info(order->m_good_id);
            if (to_user && good_info)
            {
                std::string my_comment;
                Base64::Decode(comment, &my_comment);
                send_out_sub_msg(order_brief->get_pri_id(), to_user->m_openid, good_info->m_name + "(" + order->m_spec + ")", "delete", my_comment);
                ret = true;
            }
        }
    }

    return ret;
}