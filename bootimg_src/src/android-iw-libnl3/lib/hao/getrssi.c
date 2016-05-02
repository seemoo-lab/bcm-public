#include <android/log.h>
#include <jni.h>
#include <netlink/netlink.h>
#include <netlink/cache.h>
#include <netlink/genl/family.h>
#include <netlink/msg.h>
#include <netlink/handlers.h>

#define LOG_TAG "haogetrssi"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

jint
Java_com_hao_getrssi_GetRSSI_getrssi( JNIEnv* env, jobject thiz){
	struct nl_sock *p_nl_sock = NULL;
	struct nl_cache *p_nl_cache = NULL;
	struct genl_family *p_nl_family = NULL;
	struct nl_cb *p_nl_cb = NULL;
	struct nl_msg *p_msg = NULL;
	LOGD("INITIAL");
	return 0;
}

