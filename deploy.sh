#!/bin/bash

GAME_DELIVER="dg_deliver.tar.gz"
WECHAT_SECRET_INPUT=""
PORT=80
DATA_BASE="user_cash.db"
IMG_BED_INPUT="logo_res"

DOCKER_IMG_NAME="game_deploy:v1.0"
SRC_DIR=`dirname $(realpath $0)`
is_in_container() {
    cat /proc/1/cgroup | grep pids | grep docker 2>&1>/dev/null
}

make_docker_img_from_dockerfile() {
    docker build -t ${DOCKER_IMG_NAME} ${SRC_DIR}/depend_docker
}

get_docker_image() {
    docker images ${DOCKER_IMG_NAME} | grep game_deploy > /dev/null
    if [ $? != 0 ]
    then
        make_docker_img_from_dockerfile
    fi
}

start_all_server() {
    line=`wc -l $0|awk '{print $1}'`
    line=`expr $line - 82` 
    tail -n $line $0 | tar zx  --skip-old-files -C /
    nginx -c /conf/nginx.conf
    /root/.ngrest/ngrest-build/deploy/bin/ngrestserver -s /lib &
    bash
}

start_docker_con() {
    local DATA_BASE_PATH=`realpath $DATA_BASE`
    local DATA_BASE_PATH=`dirname ${DATA_BASE_PATH}`
    local IMG_BED=`realpath $IMG_BED_INPUT`
    local CON_ID=`docker create -ti -p ${PORT}:80 -e WECHAT_SECRET="${WECHAT_SECRET_INPUT}"  -v ${DATA_BASE_PATH}:/database -v ${IMG_BED}:/dist/logo_res ${DOCKER_IMG_NAME} /root/install.sh`
    docker cp $0 ${CON_ID}:/root/
    docker cp /etc/localtime ${CON_ID}:/etc/localtime
    docker cp /etc/timezone ${CON_ID}:/etc/timezone
    docker start -ai ${CON_ID}
}

while getopts "D:p:w:d:i:" arg
do
    case $arg in
        D)
            GAME_DELIVER=${OPTARG}
            ;;
        p)
            PORT=${OPTARG}
            ;;
        w)
            WECHAT_SECRET_INPUT=${OPTARG}
            ;;
        d)
            DATA_BASE=${OPTARG}
            ;;
        i)
            IMG_BED_INPUT=${OPTARG}
            ;;
        *)
            echo "invalid args"
            ;;
    esac
done

if is_in_container 
then
    start_all_server
else
    get_docker_image
    start_docker_con
fi

#
exit



