#include <jvmti.h>
#include <jni.h>
#include <iostream>
#include <cassert>

#define CHECK_ERROR(EXPR) assert(!(EXPR))

jvmtiEnv *jvmti = nullptr;
static bool initialized = false;

std::string getClassName(JNIEnv *jni, jclass klass)
{
    jclass classObj = jni->FindClass("java/lang/Class");
    jmethodID getName = jni->GetMethodID(classObj, "getName", "()Ljava/lang/String;");
    jstring className_J = (jstring) jni->CallObjectMethod(klass, getName);

    const char* className_C = jni->GetStringUTFChars(className_J, nullptr);
    std::string className(className_C);

    jni->ReleaseStringUTFChars(className_J, className_C);

    return className;
}

void JNICALL whenClassLoaded(jvmtiEnv *jvmti, JNIEnv *jni, jthread th, jclass klass)
{
    if (!initialized) { return; }
    std::string className = getClassName(jni, klass);
    jclass superKlass = jni->GetSuperclass(klass);

    if (superKlass != nullptr) {
        std::string superClassName = getClassName(jni, superKlass);
        std::cerr << className << " < " << superClassName << std::endl;
    }
    
}

void JNICALL whenInitialized(jvmtiEnv *jvmti, JNIEnv *jni, jthread th)
{
    initialized = true;
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved)
{
    jvmtiEventCallbacks callbacks;

    (void) options;
    (void) reserved;
    
    jvm->GetEnv((void**) &jvmti, JVMTI_VERSION_1_2);
    
    CHECK_ERROR(jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_START, nullptr));
    CHECK_ERROR(jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE, nullptr));
    CHECK_ERROR(jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, nullptr));
    
    callbacks.ClassPrepare = &whenClassLoaded;
    callbacks.VMInit = &whenInitialized;
    CHECK_ERROR(jvmti->SetEventCallbacks(&callbacks, (jint) sizeof(callbacks)));

    return JNI_OK;
}
