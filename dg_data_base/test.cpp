#include "dg_data_opt.h"

int main(int argc, char const *argv[])
{
    if (argv[1] == "ssid")
    {
        std::cout << get_online_user_info(argv[2])->m_name << std::endl;
    }
    else if (argv[1] == "code")
    {
        std::cout << dg_wechat_login(argv[2]) << std::endl;
    }
    
    std::cout << dg_wx_sign("Wm3WZYTPz0wzccnW", 1414587457, "http://mp.weixin.qq.com?params=value");
    return 0;
}
