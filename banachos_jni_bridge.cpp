/**
 * ============================================
 * BANACHOS v8.3 — COMPATIBLE NDK JNI BRIDGE
 * Author: Efim S. Markov & AI Collaborator
 * Formalism: Pure Bare-Metal Android NDK HAL
 * ============================================
 */

#include <jni.h>
#include <string>
#include <sstream>
#include "banachos_mobile_extensions.hpp"

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_banachos_app_AiStudio_processVoiceCommand(
    JNIEnv* env, jobject thiz, jstring prompt) 
{
    (void)thiz;
    
    // 1. Извлекаем речь пользователя из интерфейса APK
    const char* native_prompt = 
        env->GetStringUTFChars(prompt, nullptr);
    std::string prompt_str(native_prompt);
    env->ReleaseStringUTFChars(prompt, native_prompt);

    // 2. Инициализируем ячейку Пространства Всерод (0/0)
    RybnikovElectroAtom atom;
    OnDeviceCompilerAI ai_compiler;

    // 3. Цепная реакция наоборот: переводим речь в приРОДный код уплотнения
    PrirodnyAtomCode element_code = 
        ai_compiler.compile_prompt_to_prirod_code(prompt_str);
    ai_compiler.apply_matrix_strain(atom, element_code);

    // 4. Формируем точную телеметрию V256 для экрана смартфона
    std::stringstream response_stream;
    response_stream << "[V256-JNI] Успешно! Элемент: " 
                    << atom.atom_name << "\n"
                    << " -> Объемный заряд: " 
                    << atom.charge_density << "\n"
                    << " -> Винтовой шаг (оси): " 
                    << (int)atom.axes_pressure[0];

    return env->NewStringUTF(
        response_stream.str().c_str()
    );
}

}
