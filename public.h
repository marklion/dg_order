#if !defined(_PUBLIC_H_)
#define _PUBLIC_H_

#include "tcp_framework/tdf_include.h"

#define GAME_PORT 54300
#define GAME_API_PORT 54301
enum game_msg_type
{
    game_msg_type_sync_session,
    game_msg_type_mng_result,
    game_msg_type_create_table,
    game_msg_type_wechat_login,
    game_msg_type_user_login_resp,
    game_msg_type_get_user_info,
    game_msg_type_get_user_info_resp,
    game_msg_type_logoff_user,
    game_msg_type_add_cash,
    game_msg_type_create_table_resp,
    game_msg_type_enter_table,
    game_msg_type_table_info_sync,
    game_msg_type_qq_login_req,
    game_msg_type_player_sit_down,
    game_msg_type_player_self_info,
    game_msg_type_player_stand_up,
    game_msg_type_random_login,
    game_msg_type_self_hand_card,
    game_msg_type_player_action,
    game_msg_type_max
};

#endif // _PUBLIC_H_
