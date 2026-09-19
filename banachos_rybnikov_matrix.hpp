/**
 * ================================= VER. 8.3.7
 * BANACHOS — RYBNIKOV-MARKOV ATOMIC ENGINE PART 1
 * Strictly Zero-Fit Parameter Architecture
 * ============================================
 */

#ifndef BANACHOS_RYBNIKOV_MATRIX_HPP
#define BANACHOS_RYBNIKOV_MATRIX_HPP

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cmath>

struct RybnikovElectroAtom {
    std::array<uint8_t, 12> axes_pressure;
    float calculated_leeway;               
    float charge_density;                  
    int valence_mod;                       
    bool is_pure_current;                  
    std::string atom_name;                 

    // МЕТРИКИ ИЗ МОНОГРАФИИ МАРКОВА
    float weight;                          
    float electro_conductivity;            
    float phase_screwing;                  
    float thermal_conductivity;            
    float refractive_index;                
    float emergent_thrust;                 
    float thermal_background;              
    float collatz_drift;                   
    float osmotic_drag;                    
    float shear_viscosity;                 
    float harmonic_string_index;           
    std::string density_state;             
};

class BanachOSRybnikovMatrix {
public:
    static constexpr float L_E0 = 0.024f;         
    static constexpr float ZETA = 1.024f;         
    static constexpr float C_ETHER = 299792.458f; 
    static constexpr float LAMBDA_DRIFT = -0.2876f;
    static constexpr float T_BASE = 2.725f;        
    static constexpr float K_EXT = 0.95f;          
    static constexpr float R_COSMOS = 144000.0f;   
    static constexpr float MU_SHEAR = 0.015f;      
    static constexpr int N_HARMONICS = 144;        
    static constexpr int SIZE = 16;

    BanachOSRybnikovMatrix() {}
    /**
     * Сквозной расчет динамических свойств.
     * Полностью синхронизирован с UMM v9.0.
     */
    void compute_and_transfer_atomic_properties(
        RybnikovElectroAtom& target_atom, 
        const std::array<RybnikovElectroAtom, 12>& neighbors,
        float r_distance = 1000.0f) 
    {
        uint32_t total_axes_stress = 0;
        uint32_t anisotropy_gradient = 0;

        for (int i = 0; i < 12; ++i) {
            total_axes_stress += 
                target_atom.axes_pressure[i];
            if (i > 0) {
                anisotropy_gradient += 
                    std::abs(target_atom.axes_pressure[i] - 
                             target_atom.axes_pressure[i - 1]);
            }
        }

        target_atom.charge_density = 
            static_cast<float>(total_axes_stress) / 12.0f;
        target_atom.collatz_drift = LAMBDA_DRIFT;

        // 1. Осмотическое сопротивление
        if (r_distance >= R_COSMOS) 
            r_distance = R_COSMOS - 1.0f;
        target_atom.osmotic_drag = 
            1.0f / (1.0f - (r_distance / R_COSMOS));

        float sum_neighbor_density = 0.0f;
        for (int m = 0; m < 12; ++m) {
            sum_neighbor_density += 
                neighbors[m].charge_density;
        }
        float p_external = (K_EXT / 12.0f) * 
            sum_neighbor_density * 
            (target_atom.osmotic_drag / 2.0f);
        if (p_external < 0.01f) p_external = 0.01f;

        // --- ВЕРИФИКАЦИЯ ИНВЕРСИИ (256 ≡ 0) ---
        if (total_axes_stress >= 256 || 
            target_atom.charge_density >= 255.0f || 
            p_external >= 255.0f) 
        {
            target_atom.is_pure_current = true;
            target_atom.valence_mod = 0;
            target_atom.atom_name = "Ток (Инверсия)";
            target_atom.density_state = "ИНВЕРСИЯ";
            target_atom.calculated_leeway = 0.1f;       
            target_atom.weight = 0.0f;                  
            target_atom.phase_screwing = 0.0f;
            target_atom.electro_conductivity = 1000.0f; 
            target_atom.thermal_conductivity = 0.0f;
            target_atom.refractive_index = 1.0f;
            target_atom.emergent_thrust = 100.0f;
            target_atom.thermal_background = 0.0f;
            target_atom.shear_viscosity = 1000.0f; 
            target_atom.harmonic_string_index = 143.0f;
            return;
        }

        target_atom.is_pure_current = false;
        target_atom.valence_mod = 
            static_cast<int>(
                std::floor(target_atom.charge_density)
            ) % 256;

        // Классификация Родов по Ю.С. Рыбникову
        if (target_atom.valence_mod <= 1)       
            { target_atom.atom_name = "Всерод"; }
        else if (target_atom.valence_mod <= 2)  
            { target_atom.atom_name = "Первород"; }
        else if (target_atom.valence_mod <= 4)  
            { target_atom.atom_name = "Двурод"; }
        else if (target_atom.valence_mod <= 8)  
            { target_atom.atom_name = "Трирод"; }
        else if (target_atom.valence_mod <= 16) 
            { target_atom.atom_name = "Четверород"; }
        else if (target_atom.valence_mod <= 32) 
            { target_atom.atom_name = "Пятирод"; }
        else if (target_atom.valence_mod <= 64) 
            { target_atom.atom_name = "Шестирод"; }
        else if (target_atom.valence_mod <= 128)
            { target_atom.atom_name = "Семирод"; }
        else                             
            { target_atom.atom_name = "Высший Род"; }

        float p_internal = 1.0f * 
            ((256.0f - target_atom.charge_density) / 
             (target_atom.charge_density + 1.0f));
        if (p_internal < 0.0f) p_internal = 0.0f;

        // --- ДИНАМИЧЕСКИЙ БАЛАНС ДАВЛЕНИЙ ---
        float stress_factor = 
            target_atom.charge_density / 256.0f;

        if (p_internal > p_external) {
            // РАЗУПЛОТНЕНИE (Rarefaction)
            target_atom.density_state = "RAREFACTION";
            float rarefaction_factor = 
                (p_internal - p_external) / 
                (p_internal + 1.0f);
            target_atom.calculated_leeway = L_E0 * 
                (1.0f + rarefaction_factor * (ZETA - 1.0f));
            if (target_atom.calculated_leeway > L_E0 * ZETA) 
                target_atom.calculated_leeway = L_E0 * ZETA;
            
            target_atom.weight = 0.0f; 
            target_atom.electro_conductivity = 100.0f; 
            target_atom.refractive_index = 1.0f; 
            target_atom.thermal_background = T_BASE * 
                std::pow((target_atom.charge_density + 0.001f), 
                         0.024f);
            target_atom.shear_viscosity = MU_SHEAR * L_E0; 
        } else {
            // УПЛОТНЕНИЕ (Compression)
            float compression_ratio = 
                p_external / (p_internal + 0.001f);
            target_atom.density_state = "COMPRESSION";
            
            target_atom.calculated_leeway = L_E0 * 
                (1.0f - (compression_ratio / 256.0f) * ZETA) * 
                std::exp(target_atom.collatz_drift * 0.01f);
            if (target_atom.calculated_leeway < 0.0f) 
                target_atom.calculated_leeway = 0.0f;
            
            // Вес (M_baryon по формуле 47)
            target_atom.weight = target_atom.charge_density * 
                (1.0f - (target_atom.calculated_leeway / L_E0)) * 
                ZETA * (p_external / 10.0f);
            target_atom.electro_conductivity = 
                (target_atom.calculated_leeway / L_E0) * 100.0f;
            
            target_atom.refractive_index = 1.0f / 
                (1.0f - 0.95f * stress_factor);
            if (target_atom.refractive_index > 20.0f) 
                target_atom.refractive_index = 20.0f;
            
            target_atom.thermal_background = T_BASE * 
                std::pow(target_atom.weight / 1e-6f, 0.024f) * 
                (1.0f + L_E0) * (p_external);

            // Вязкость Маркова (Уравнение 81)
            float saturation_gap = 1.0f - stress_factor;
            if (saturation_gap < 0.001f) saturation_gap = 0.001f;
            target_atom.shear_viscosity = MU_SHEAR * 
                (L_E0 - target_atom.calculated_leeway) * 
                (1.0f / saturation_gap);
        }

        // --- ГЛАВА 9.4: СЖАТИЕ ЧАСТОТЫ СТРУНЫ S_m ---
        float k_comp = L_E0 / 
            (target_atom.calculated_leeway + 1e-6f);
        float m_target = 
            static_cast<float>(target_atom.valence_mod) * 
            (static_cast<float>(N_HARMONICS) / 256.0f);
        float log_scale = 
            std::log10(static_cast<float>(N_HARMONICS) * 
                       k_comp + 1.0f);
        target_atom.harmonic_string_index = 
            std::pow(10.0f, (m_target / (N_HARMONICS - 1)) * 
                     log_scale) - 1.0f;
        if (target_atom.harmonic_string_index > N_HARMONICS - 1) 
            target_atom.harmonic_string_index = 
                static_cast<float>(N_HARMONICS - 1);
        if (target_atom.harmonic_string_index < 0.0f) 
            target_atom.harmonic_string_index = 0.0f;

        target_atom.phase_screwing = 
            static_cast<float>(anisotropy_gradient) * 
            ZETA * 0.024f;
        target_atom.thermal_conductivity = C_ETHER * 
            (target_atom.calculated_leeway / L_E0);
        target_atom.emergent_thrust = 
            static_cast<float>(anisotropy_gradient) * 
            std::abs(L_E0 - target_atom.calculated_leeway) * 
            500.0f;
    }
};

#endif // BANACHOS_RYBNIKOV_MATRIX_HPP
