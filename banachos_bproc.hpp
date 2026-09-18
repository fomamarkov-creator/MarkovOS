    /**
     * Вычисление нелинейного липшицева зазора упругости шины bPROC (Модификация v8.3.1)
     * Интегрирует модуль сдвиговой эластичности MU_SHEAR и динамическую экспансию ячейки.
     */
    float calculate_elastic_scheduler_leeway(float p_ext, float p_in, float local_charge) const {
        if (p_in <= 0.0f) return 0.0f;

        const float MU_SHEAR = 0.85f; // Коэффициент сдвиговой эластичности All-Born матрицы
        float pressure_ratio = (p_ext - p_in) / p_in;

        // Эластичный отклик зазора по Маркову: учитывает сопротивление сдвигу окружающих ячеек
        float current_le = LE_BASE * (1.0f - MU_SHEAR * pressure_ratio);

        // --- ЭФФЕКТ МИКРО-ЭКСПАНСИИ ЯЧЕЙКИ ПРОСТРАНСТВА ---
        // Если внутреннее давление растет, сфера упруго деформируется наружу, временно увеличивая локальный люфт
        if (local_charge > 10.0f) {
            float expansion_factor = std::log1p(local_charge / 256.0f) * 0.05f;
            current_le += expansion_factor; // Локальное расширение зазора упругости соты
        }

        return std::clamp(current_le, 0.0f, 0.1f); // Верхний упругий ограничитель предела Гука
    }
