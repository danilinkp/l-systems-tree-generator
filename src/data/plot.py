# import pandas as pd
# import matplotlib.pyplot as plt
# import numpy as np
# import os

# def parse_and_plot(csv_file_path, output_dir='output'):
#     """
#     Parses a single benchmark CSV file based on column values and creates 3 separate CSV files and plots.

#     Args:
#         csv_file_path (str): Path to the input CSV file.
#         output_dir (str): Directory to save output CSVs and plots.
#     """
#     os.makedirs(output_dir, exist_ok=True)

#     print(f"Reading data from {csv_file_path}...")
#     df = pd.read_csv(csv_file_path)

#     # --- Stage 1: Iterations (n={2,3,4,5,6}) ---
#     df_stage1 = df[
#         (df['wigth'] == 1920) &
#         (df['height'] == 1080) &
#         (df['shadowmap'] == 2048)
#         ].copy()

#     if not df_stage1.empty:
#         # Выбираем нужные столбцы, исключая постоянные параметры
#         df_stage1_output = df_stage1[['Iterations', 'GenerationTime(ms)', 'RenderTime(ms)', 'Triangles', 'Leaves']].copy()

#         # Сохраняем в CSV
#         stage1_csv_path = os.path.join(output_dir, 'stage1_iterations_result.csv')
#         df_stage1_output.to_csv(stage1_csv_path, index=False)
#         print(f"Saved Stage 1 data to {stage1_csv_path}")

#         # Строим ГРАФИКИ для Stage 1: Generation и Render отдельно
#         grouped = df_stage1_output.groupby('Iterations').agg({
#             'GenerationTime(ms)': 'mean',
#             'RenderTime(ms)': 'mean',
#             'Triangles': 'mean',
#             'Leaves': 'mean'
#         }).reset_index()

#         # 1. График Generation Time
#         fig_gen, ax_gen = plt.subplots(figsize=(10, 6))
#         ax_gen.plot(grouped['Iterations'], grouped['GenerationTime(ms)'], marker='o', label='Время генерации дерева', color='blue')
#         ax_gen.set_xlabel('Количество итераций L-системы)')
#         ax_gen.set_ylabel('Время, мс')
#         ax_gen.grid(True)
#         ax_gen.set_xticks(grouped['Iterations'])
#         ax_gen.legend()
#         plt.tight_layout()
#         stage1_gen_plot_path = os.path.join(output_dir, 'stage1_gen_time_plot.pdf')
#         plt.savefig(stage1_gen_plot_path)
#         print(f"Saved Stage 1 Generation Time plot to {stage1_gen_plot_path}")
#         plt.close(fig_gen)

#         # 2. График Render Time
#         fig_render, ax_render = plt.subplots(figsize=(10, 6))
#         ax_render.plot(grouped['Iterations'], grouped['RenderTime(ms)'], marker='s', label='Время рендера изображения', color='red')
#         ax_render.set_xlabel('Количество итераций L-системы')
#         ax_render.set_ylabel('Время, мс')
#         ax_render.grid(True)
#         ax_render.legend()
#         plt.tight_layout()
#         stage1_render_plot_path = os.path.join(output_dir, 'stage1_render_time_plot.pdf')
#         plt.savefig(stage1_render_plot_path)
#         print(f"Saved Stage 1 Render Time plot to {stage1_render_plot_path}")
#         plt.close(fig_render)

#     else:
#         print("Warning: No data found for Stage 1 (Iterations).")

#     # --- Stage 2: Resolution (r) at fixed Iterations (e.g., 5) ---
#     fixed_iter_s2 = 5 # Укажите ожидаемое значение итераций для этапа 2
#     df_stage2 = df[
#     (df['Iterations'] == fixed_iter_s2) & # <-- Используем фиксированное значение итераций
#     (df['shadowmap'] == 2048) # Теневая карта постоянна
#     ].copy() # Убираем отрицание ~

#     # unique_iterations_s2 = df_stage2_candidates['Iterations'].unique()
#     # if len(unique_iterations_s2) == 1:
#     #     fixed_iter_s2 = unique_iterations_s2[0]
#     #     df_stage2 = df_stage2_candidates[df_stage2_candidates['Iterations'] == fixed_iter_s2].copy()
#     # else:
#     #     print(f"Warning: Iterations are not fixed for Stage 2 candidates: {unique_iterations_s2}. Cannot process Stage 2.")
#     #     df_stage2 = pd.DataFrame()

#     if not df_stage2.empty:
#         # Создаем столбец с разрешением в формате "WxH"
#         df_stage2['Resolution'] = df_stage2['wigth'].astype(str) + 'x' + df_stage2['height'].astype(str)
#         # Выбираем нужные столбцы (RenderTime только)
#         df_stage2_output = df_stage2[['Resolution', 'RenderTime(ms)']].copy()

#         # Сохраняем в CSV
#         stage2_csv_path = os.path.join(output_dir, 'stage2_resolution_result.csv')
#         df_stage2_output.to_csv(stage2_csv_path, index=False)
#         print(f"Saved Stage 2 data to {stage2_csv_path}")

#         # Строим график: Render Time от разрешения
#         fig_res, ax_res = plt.subplots(figsize=(12, 6))
#         df_stage2_output['Resolution'] = pd.Categorical(df_stage2_output['Resolution'],
#                                                         categories=['256x144', '426x240', '640x360', '854x480', '1280x720', '1920x1080', '2560x1440'],
#                                                         ordered=True)
#         grouped_res = df_stage2_output.groupby('Resolution', observed=True).agg({
#             'RenderTime(ms)': 'mean'
#         }).reset_index()

#         grouped_res['Resolution_Str'] = grouped_res['Resolution'].astype(str)
#         ax_res.plot(grouped_res['Resolution_Str'], grouped_res['RenderTime(ms)'], marker='s', label='Время рендера изображения', color='red')
#         ax_res.set_xlabel('Разрешение экрана')
#         ax_res.set_ylabel('Время, мс')
#         ax_res.grid(True)
#         ax_res.tick_params(axis='x')
#         ax_res.legend()
#         plt.tight_layout()
#         stage2_plot_path = os.path.join(output_dir, 'stage2_resolution_plot.pdf')
#         plt.savefig(stage2_plot_path)
#         print(f"Saved Stage 2 plot to {stage2_plot_path}")
#         plt.close(fig_res)
#     else:
#         print("Warning: No data found for Stage 2 (Resolution).")

#     # --- Stage 3: Shadow Map Resolution (s) at fixed Iterations (e.g., 5) and fixed Resolution (1920x1080) ---
#     fixed_iter_s3 = 5 # Укажите ожидаемое значение итераций для этапа 3
#     df_stage3 = df[
#     (df['Iterations'] == fixed_iter_s3) & # <-- Используем фиксированное значение итераций
#     (df['wigth'] == 1920) &
#     (df['height'] == 1080)
#     # Убираем фильтр !(df['shadowmap'] != 2048), теперь все shadowmap при 1920x1080 и Iterations=5 попадают
#     ].copy()

#     # unique_iterations_s3 = df_stage3_candidates['Iterations'].unique()
#     # if len(unique_iterations_s3) == 1:
#     #     fixed_iter_s3 = unique_iterations_s3[0]
#     #     df_stage3 = df_stage3_candidates[df_stage3_candidates['Iterations'] == fixed_iter_s3].copy()
#     # else:
#     #     print(f"Warning: Iterations are not fixed for Stage 3 candidates: {unique_iterations_s3}. Cannot process Stage 3.")
#     #     df_stage3 = pd.DataFrame()

#     if not df_stage3.empty:
#         # Выбираем нужные столбцы (RenderTime только)
#         df_stage3_output = df_stage3[['shadowmap', 'RenderTime(ms)']].copy()
#         # Преобразуем в строку для равномерного отображения
#         df_stage3_output['ShadowMapResolution'] = df_stage3_output['shadowmap'].astype(str)

#         # Сохраняем в CSV
#         stage3_csv_path = os.path.join(output_dir, 'stage3_shadowmap_result.csv')
#         df_stage3_output[['ShadowMapResolution', 'RenderTime(ms)']].to_csv(stage3_csv_path, index=False)
#         print(f"Saved Stage 3 data to {stage3_csv_path}")

#         # Строим график: Render Time от разрешения теневой карты (равномерно по X)
#         fig_sm, ax_sm = plt.subplots(figsize=(10, 6))
#         grouped_sm = df_stage3_output.groupby('ShadowMapResolution').agg({
#             'RenderTime(ms)': 'mean'
#         }).reset_index()

#         # Явно задаём порядок категорий, если нужен определённый (опционально)
#         expected_order = ['128', '256', '512', '1024', '2048', '4096']  # ← замените на ваши реально используемые значения
#         # Фильтруем и сортируем по ожидаемому порядку, чтобы избежать перемешивания
#         grouped_sm['ShadowMapResolution'] = pd.Categorical(
#             grouped_sm['ShadowMapResolution'],
#             categories=[x for x in expected_order if x in grouped_sm['ShadowMapResolution'].values],
#             ordered=True
#         )
#         grouped_sm = grouped_sm.sort_values('ShadowMapResolution')

#         ax_sm.plot(
#             grouped_sm['ShadowMapResolution'].astype(str),
#             grouped_sm['RenderTime(ms)'],
#             marker='s',
#             label='Время построения изображения',
#             color='blue'
#         )
#         ax_sm.set_xlabel('Размер теневой карты')
#         ax_sm.set_ylabel('Время, мс')
#         ax_sm.set_xticks(grouped_sm['ShadowMapResolution'].astype(str))  # гарантируем отображение всех меток
#         ax_sm.grid(True)
#         ax_sm.legend()
#         plt.tight_layout()
#         stage3_plot_path = os.path.join(output_dir, 'stage3_shadowmap_plot.pdf')
#         plt.savefig(stage3_plot_path)
#         print(f"Saved Stage 3 plot to {stage3_plot_path}")
#         plt.close(fig_sm)
#     else:
#         print("Warning: No data found for Stage 3 (ShadowMapResolution).")

#     print("\nProcessing completed. Output files are in the '{}' directory.".format(output_dir))

# if __name__ == "__main__":
#     # Путь к входному CSV файлу
#     input_csv_file = "benchmark_result.csv" # Измените на реальный путь
#     output_directory = "parsed_output"      # Измените на нужную папку вывода

#     parse_and_plot(input_csv_file, output_directory)

import pandas as pd
import matplotlib.pyplot as plt

# Встроенные данные
data = {
    'ShadowMapResolution': [128, 256, 512, 1024, 2048, 4096],
    'RenderTime(ms)': [430.055, 434.065, 443.957, 466.171, 533.541, 763.706]
}

# Создаём DataFrame
df = pd.DataFrame(data)

# Явно задаём порядок (он уже правильный, но для надёжности)
expected_order = [128, 256, 512, 1024, 2048, 4096]
df['ShadowMapResolution'] = pd.Categorical(
    df['ShadowMapResolution'],
    categories=expected_order,
    ordered=True
)
df = df.sort_values('ShadowMapResolution')

# Строим график
fig, ax = plt.subplots(figsize=(10, 6))

ax.plot(
    df['ShadowMapResolution'].astype(str),
    df['RenderTime(ms)'],
    marker='s',
    label='Время построения изображения',
    color='blue'
)

ax.set_xlabel('Размер теневой карты')
ax.set_ylabel('Время, мс')
ax.set_xticks(df['ShadowMapResolution'].astype(str))
ax.grid(True)
ax.legend()

plt.tight_layout()

# Сохраняем
output_plot_path = 'parsed_output/stage3_shadowmap_plot.svg'
plt.savefig(output_plot_path)
print(f"График сохранён в {output_plot_path}")

plt.close(fig)