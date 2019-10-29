
typedef struct xC11nTaskParameters {
    int port;
    char *ssid;
    char *password;
    char *uri;
} xC11nTaskParameters;

void vTaskCommunication(void *pvParameters);
