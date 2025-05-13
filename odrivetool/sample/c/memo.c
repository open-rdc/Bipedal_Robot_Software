#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct ifreq ifr;

    // ソケット作成
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // インターフェース名の設定
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

    // MTU取得
    if (ioctl(sockfd, SIOCGIFMTU, &ifr) == -1) {
        perror("ioctl");
        close(sockfd);
        return 1;
    }

    printf("MTU of %s: %d\n", ifr.ifr_name, ifr.ifr_mtu);
    
    // ソケット閉じる
    close(sockfd);
    return 0;
}
