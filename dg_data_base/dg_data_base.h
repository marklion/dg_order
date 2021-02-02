#if !defined(_DG_DATA_BASE_H_)
#define _DG_DATA_BASE_H_

#include "../public.h"
#include "../sqlite_orm/sqlite_orm.h"
#define DG_DB_FILE "/database/dg.db"

class dg_db_user_info:public sqlite_orm {
public:
    std::string m_openid;
    std::string m_name;
    std::string m_logo;
    dg_db_user_info(const std::string &_file):sqlite_orm(_file) {}
    std::vector<sqlite_orm_column> columns_defined() {
        std::vector<sqlite_orm_column> ret;

        ret.push_back(sqlite_orm_column("openid", sqlite_orm_column::STRING, &m_openid, SQLITE_ORM_COLUMN_LIMIT_UNIQ));
        ret.push_back(sqlite_orm_column("name", sqlite_orm_column::STRING, &m_name));
        ret.push_back(sqlite_orm_column("logo", sqlite_orm_column::STRING, &m_logo));

        return ret;
    }

    std::string table_name() {
        return "user_info";
    }
};

class dg_db_user_login:public sqlite_orm {
public:
    dg_db_user_login(const std::string& _file) : sqlite_orm(_file) {}
    std::string m_ssid;
    int m_user_info_id = 0;
    int m_login_timestamp_hour = 0;

    std::vector<sqlite_orm_column> columns_defined() {
        std::vector<sqlite_orm_column> ret;

        ret.push_back(sqlite_orm_column("ssid", sqlite_orm_column::STRING, &m_ssid, SQLITE_ORM_COLUMN_LIMIT_UNIQ));
        ret.push_back(sqlite_orm_column("user_info_id", sqlite_orm_column::INTEGER, &m_user_info_id, SQLITE_ORM_COLUMN_LIMIT_UNIQ));
        ret.push_back(sqlite_orm_column("timestamp", sqlite_orm_column::INTEGER, &m_login_timestamp_hour));

        return ret;
    }

    std::string table_name() {
        return "user_login";
    }
};

class dg_db_order_owner:public sqlite_orm {
public:
    int m_owner_user_id;
    std::string m_destination;
    std::string m_start_time;
    std::string m_deliver_time;
    std::string m_comments;
    dg_db_order_owner(const std::string& _sql_file) : sqlite_orm(_sql_file) {}

    std::vector<sqlite_orm_column> columns_defined() {
        std::vector<sqlite_orm_column> ret;

        ret.push_back(sqlite_orm_column("owner_user_id", sqlite_orm_column::INTEGER, &m_owner_user_id));
        ret.push_back(sqlite_orm_column("destination", sqlite_orm_column::STRING, &m_destination));
        ret.push_back(sqlite_orm_column("start_time", sqlite_orm_column::STRING, &m_start_time));
        ret.push_back(sqlite_orm_column("deliver_time", sqlite_orm_column::STRING, &m_deliver_time));
        ret.push_back(sqlite_orm_column("comments", sqlite_orm_column::STRING, &m_comments));

        return ret;
    }
    std::string table_name() {
        return "order_owner";
    }
};

class dg_db_goods:public sqlite_orm {
public:
    std::string m_spec;
    int m_user_id = 0;
    int m_order_id = 0;
    int m_good_id = 0;
    std::string m_status = "booking";
    dg_db_goods(const std::string &_sql_file):sqlite_orm(_sql_file) {}

    std::vector<sqlite_orm_column> columns_defined() {
        std::vector<sqlite_orm_column> ret;

        ret.push_back(sqlite_orm_column("spec", sqlite_orm_column::STRING, &m_spec));
        ret.push_back(sqlite_orm_column("user_id", sqlite_orm_column::INTEGER, &m_user_id));
        ret.push_back(sqlite_orm_column("order_id", sqlite_orm_column::INTEGER, &m_order_id));
        ret.push_back(sqlite_orm_column("good_id", sqlite_orm_column::INTEGER, &m_good_id));
        ret.push_back(sqlite_orm_column("status", sqlite_orm_column::STRING, &m_status));

        return ret;
    }

    std::string table_name() {
        return "goods_order";
    }
};

class dg_db_good_info:public sqlite_orm {
public:
    std::string m_name;
    std::string m_picture;
    dg_db_good_info(const std::string &_sqlite_file):sqlite_orm(_sqlite_file) {}
    std::vector<sqlite_orm_column> columns_defined() {
        std::vector<sqlite_orm_column> ret;

        ret.push_back(sqlite_orm_column("name", sqlite_orm_column::STRING, &m_name, SQLITE_ORM_COLUMN_LIMIT_UNIQ));
        ret.push_back(sqlite_orm_column("picture", sqlite_orm_column::STRING, &m_picture));

        return ret;
    }

    std::string table_name() {
        return "good_info";
    }
};




#endif // _DG_DATA_BASE_H_
