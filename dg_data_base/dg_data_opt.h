#if !defined(_DG_DATA_OPT_H)
#define _DG_DATA_OPT_H

#include "../public.h"
#include <memory>
#include "dg_data_base.h"
#include <functional>

std::unique_ptr<dg_db_user_info> get_online_user_info(const std::string& _ssid);
std::unique_ptr<dg_db_user_info> get_user_info(const std::string& _openid);
std::unique_ptr<dg_db_user_info> get_user_info(int _user_info_id);
std::string dg_wechat_login(const std::string& _code);

std::string dg_create_order(int _user_id, const std::string& _destination, const std::string& _start_time, const std::string& _deliver_time, const std::string& _commonts);
std::unique_ptr<dg_db_order_owner> dg_get_order(const std::string& _order_id);

void dg_get_goods_name_by_order_id(const std::string& _order_id, std::function<bool ( const dg_db_goods &)> const &f);
void dg_get_buyer_by_name(const std::string &_order_id, const std::string& _name, std::function<bool ( const dg_db_goods &)> const &f);
int dg_get_count_by_buyer_spec(const std::string &_order_id, const std::string& _name, int _user_id, const std::string &_spec);
std::string dg_insert_goods(const std::string &_good_name, const std::string &_spec, const std::string &_picture, int _oder_id, int _user_id);

const std::string dg_wx_sign(const std::string& nonceStr, long timestamp, const std::string &url);
#endif // _DG_DATA_OPT_H