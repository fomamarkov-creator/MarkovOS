package com.banachos.app;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

/**
 * ============================================
 * BANACHOS v8.3 — MAIN APK INTERFACE VIEW
 * Author: Efim S. Markov & AI Collaborator
 * Formalism: Pure Bare-Metal Android HAL
 * ============================================
 */
public class MainActivity extends Activity {

    // Элементы интерфейса V256
    private EditText promptInput;
    private TextView telemetryOutput;
    private Button compileButton;

    // Подгрузка нативного C++ монолита ядра
    static {
        System.loadLibrary("banachos_core");
    }

    // Объявление JNI-метода из jni_bridge.cpp
    public native String processVoiceCommand(
        String prompt
    );

    @Override
    protected void onCreate(Bundle saved) {
        super.onCreate(saved);
        
        // Фоновая инициализация разметки экрана
        setContentView(
            R.layout.activity_main
        );

        promptInput = findViewById(
            R.id.prompt_input
        );
        telemetryOutput = findViewById(
            R.id.telemetry_output
        );
        compileButton = findViewById(
            R.id.compile_button
        );

        // Обработчик нажатия: "цепная реакция наоборот"
        compileButton.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    String userPrompt = 
                        promptInput.getText()
                        .toString();
                    
                    // Вызов C++ ядра V256
                    String result = 
                        processVoiceCommand(
                            userPrompt
                        );
                    
                    // Вывод телеметрии на экран
                    telemetryOutput.setText(
                        result
                    );
                }
            }
        );
    }
}
