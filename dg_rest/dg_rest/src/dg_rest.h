// This file generated by ngrestcg
// For more information, please visit: https://github.com/loentar/ngrest

#ifndef DG_REST_H
#define DG_REST_H

#include <ngrest/common/Service.h>
#include <vector>


struct user_info
{
    bool online;
    std::string user_name;
    std::string user_logo;
};


struct dg_order_info
{
    std::string destination;
    std::string start_time;
    std::string deliver_time;
    std::string comments;
};

struct dg_order_show 
{
    std::string order_owner_name;
    std::string order_owner_logo;
    dg_order_info info;
};

struct dg_order_config
{
    std::string ssid;
    dg_order_info info;
};

struct dg_goods_buyer_info {
    std::string user_name;
    std::string user_logo;
    std::string spec;
    int number;
};

struct dg_goods_show
{
    std::string name;
    std::string picture;
    int total;
    std::vector<dg_goods_buyer_info> buyer;
};

struct dg_good_insert 
{
    std::string ssid;
    std::string order_id;
    std::string name;
    std::string spec;
    std::string img;
    int number;
};

//! Dummy description for the service
/*! Some detailed description of the service */
// '*location' comment sets resource path for this service
// *location: dg_rest
class dg_rest: public ngrest::Service
{
public:
    // Here is an example of service operation
    //! Dummy description for the operation
    /*! Some detailed description of the operation */
    // To invoke this operation from browser open: http://localhost:9098/dg_rest/World!
    //
    // '*location' metacomment sets path to operation relative to service operation.
    // Default value is operation name.
    // This will bind "echo" method to resource path: http://host:port/dg_rest/{text}


    // *location: /user_info/{ssid}
    // *method: GET
    user_info proc_get_user_info(const std::string& ssid);

    // *location: /wechat_login
    // *method: POST
    std::string proc_wechat_login(const std::string& code);

    // *location: /dg_order
    // *method: POST
    std::string proc_dg_order_create(const dg_order_config& order);

    // *location: /dg_order/{order_id}
    // *method: GET
    dg_order_show proc_dg_get_order(const std::string& order_id);


    // *location: /dg_order/goods/{order_id}
    // *method: GET
    std::vector<dg_goods_show> proc_dg_get_goods(const std::string& order_id);


    // *location: /dg_order/goods
    // *method: POST
    std::string proc_dg_insert_goods(const dg_good_insert& good);


    // *location: /dg_wx_sign
    // *method: POST
    std::string proc_wx_sign(const std::string& nonceStr, long timestamp, const std::string& url);
};


#endif // DG_REST_H


