#include <jni.h>
#include <string>
#include "banachos_mobile_extensions.hpp"

extern "C" JNIEXPORT jstring JNICALL
Java_com_banachos_app_AiStudio_processVoiceCommand(
    JNIEnv* env, jobject thiz, jstring prompt) 
{
    // Чтение команды из интерфейса телефона
    const char* native_prompt = 
        env->GetStringUTFChars(prompt, nullptr);
        
    RybnikovElectroAtom atom;
    OnDeviceCompilerAI ai_compiler;
    
    // Автономный перевод речи в 12 осей сжатия
    ai_compiler.compile_prompt_to_matrix(
        std::string(native_prompt), atom
    );
    
    env->ReleaseStringUTFChars(prompt, native_prompt);
    
    // Возвращаем результат прямо на экран APK
    return env->NewStringUTF(
        "Матрица Всеродов перестроена автономно!"
    );
}
