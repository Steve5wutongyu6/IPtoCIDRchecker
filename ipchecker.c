#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 将IP地址转换为32位整数
uint32_t ipToUint32(const char* ip) {
    uint32_t result = 0;
    const char* p = ip;
    
    while (*p != '\0') {
        if (*p == '.') {
            result = (result << 8) + atoi(ip);
            ip = p + 1;
        }
        p++;
    }
    
    result = (result << 8) + atoi(ip);
    return result;
}

// 判断IP是否在CIDR地址段内
bool isIpInCidr(const char* ip, const char* cidr) {
    uint32_t ipValue = ipToUint32(ip);
    
    // 解析CIDR地址段
    char* cidrIp = strdup(cidr);
    char* cidrMaskStr = strchr(cidrIp, '/');
    if (cidrMaskStr == NULL) {
        free(cidrIp);
        return false;
    }
    *cidrMaskStr++ = '\0';
    int cidrMask = atoi(cidrMaskStr);
    uint32_t cidrMaskValue = cidrMask > 0 ? (0xFFFFFFFFU << (32 - cidrMask)) : 0;
    
    // 将CIDR地址段转换为32位整数
    uint32_t cidrIpValue = ipToUint32(cidrIp);
    uint32_t cidrStartIp = cidrIpValue & cidrMaskValue;
    uint32_t cidrEndIp = cidrStartIp | (~cidrMaskValue);
    
    free(cidrIp);
    
    // 检查IP是否在CIDR地址段内
    return (ipValue >= cidrStartIp) && (ipValue <= cidrEndIp);
}

int main() {
    char ip[16];
    
    printf("请输入IP地址：");
    scanf("%15s", ip);
    
    FILE *file;
    file = fopen("cidr.txt", "r");
    if (file == NULL) {
        printf("无法打开文件 cidr.txt\n");
        return 1;
    }
    
    bool isInCidr = false;
    
    char cidr[18];
    while (fgets(cidr, sizeof(cidr), file)) {
        cidr[strcspn(cidr, "\n")] = '\0'; // 去掉换行符

        if (isIpInCidr(ip, cidr)) {
            isInCidr = true;
            printf("%s 在 %s 内\n", ip, cidr);
            break;
        }
    }
    
    fclose(file);
    
    if (!isInCidr) {
        printf("%s 不在任何CIDR地址段内\n", ip);
    }
    
    return 0;
}
