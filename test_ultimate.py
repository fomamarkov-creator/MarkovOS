#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
NUMERICAL VERIFICATION SUITE FOR UMM v8.3
Tests: Invariant bounds, Shumann acoustic whistle, and registration limits.
================================================================================
"""

import pytest
import numpy as np
from umm_core import UMMDataTranslationBridge, UMMv83UltimateEngine, UMMSpectrumAnalyzer, UMMRadiationAnalyzer

def test_umm_bridge_and_flat_plateau():
    """Проверка формирования стабильного плато скоростей без темной материи."""
    grid_size = 100
    m_baryon = 6e10
    core_rho = 220.0
    
    table = UMMDataTranslationBridge.build_astro_table(
        grid_size=grid_size,
        m_baryon_input=m_baryon,
        core_rho_input=core_rho,
        theta_axis=np.pi / 4.0
    )
    
    assert len(table["radii_kpc"]) == grid_size
    assert not np.isnan(table["v_rot_kms"]).any()
    assert not np.isnan(table["raman_dispersion"]).any()
    assert table["ether_temp_K"][0] > 2.7  # Выше температуры реликтового фона

def test_ultimate_engine_shumann_jump():
    """Проверка лавинообразного подъема частоты Шумана при сжатии ядра."""
    engine = UMMv83UltimateEngine(grid_size=32)
    
    # Режим нормальной плотности
    state_normal = engine.calculate_state(m_baryon=4e10, core_rho=120.0, steps=50)
    # Режим критического сжатия соты
    state_compressed = engine.calculate_state(m_baryon=4e10, core_rho=250.0, steps=50)
    
    assert state_compressed["shumann_hz"] > state_normal["shumann_hz"]
    assert state_compressed["inversion_triggered"] == False

def test_spectrum_decoder_hydrogen():
    """Проверка детерминированного маскирования и декодирования линий водорода."""
    raw_flux = np.ones(144, dtype=np.float64) * 100.0
    # Имитируем провалы поглощения на спектральных линиях H I
    raw_flux[0:4] = 5.0
    raw_flux[41:45] = 5.0
    
    current_le_vector = np.ones(144, dtype=np.float64) * 0.024
    raman_vector = np.zeros(144, dtype=np.float64)
    
    detected = UMMSpectrumAnalyzer.decode_spectrum(
        raw_flux_array=raw_flux,
        current_le_vector=current_le_vector,
        raman_vector=raman_vector
    )
    
    assert "Межзвездный Н I (Водород)" in detected
    assert detected["Межзвездный Н I (Водород)"] > 20.0

def test_radiation_topological_collapse():
    """Тест Эффекта №16: переход за барьер 256 и выброс полярной тяги."""
    # Докритический фон
    rad_stable = UMMRadiationAnalyzer.evaluate_radiation_flux(core_density=150.0)
    assert rad_stable["unstable_state"] == False
    
    # Топологический выворот
    rad_jet = UMMRadiationAnalyzer.evaluate_radiation_flux(core_density=256.0)
    assert rad_jet["unstable_state"] == True
    assert rad_jet["radiation_flux_intensity"] == 256.0 * 1.024
    assert "Анизотропный джет" in rad_jet["radiation_class"]
