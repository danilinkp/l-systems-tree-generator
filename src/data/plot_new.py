import pandas as pd
import matplotlib.pyplot as plt
import os

def plot_camera_angle_dependency(csv_file_path, output_dir='output'):
    os.makedirs(output_dir, exist_ok=True)

    df = pd.read_csv(csv_file_path)

    if 'CameraYaw' not in df.columns or 'RenderTime(ms)' not in df.columns:
        raise ValueError("CSV must contain 'CameraYaw' and 'RenderTime(ms)' columns.")

    df = df.sort_values('CameraYaw')

    grouped = df.groupby('CameraYaw')['RenderTime(ms)'].mean().reset_index()

    plt.figure(figsize=(10, 6))
    plt.bar(
        grouped['CameraYaw'],
        grouped['RenderTime(ms)'],
        color='purple',
        width=9.8
    )

    plt.xlabel('Угол поворота камеры, градусы')
    plt.ylabel('Время рендеринга, мс')
    plt.title('Зависимость времени рендеринга от угла обзора камеры')
    plt.xticks(grouped['CameraYaw'])

    output_path = os.path.join(output_dir, 'stage4_camera_angle_plot.pdf')
    plt.tight_layout()
    plt.savefig(output_path)
    plt.close()

    print(f"Гистограмма сохранена: {output_path}")

if __name__ == "__main__":
    input_csv = "benchmark_result.csv"
    plot_camera_angle_dependency(input_csv, output_dir="output")