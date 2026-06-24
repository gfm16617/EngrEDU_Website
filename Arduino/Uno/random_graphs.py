import sys
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout,
    QHBoxLayout, QPushButton, QComboBox, QLabel, QStatusBar
)
from PyQt5.QtCore import Qt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

plt.style.use("dark_background")


class GraphCanvas(FigureCanvas):
    def __init__(self, parent=None, width=10, height=8, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi, facecolor="#1e1e1e")
        super().__init__(self.fig)
        self.setParent(parent)
        self._axes = []

    def clear(self):
        self.fig.clear()
        self._axes = []

    def plot_random(self, graph_type: str, count: int = 4):
        self.clear()
        cols = max(1, int(np.ceil(np.sqrt(count))))
        rows = max(1, int(np.ceil(count / cols)))
        for i in range(count):
            ax = self.fig.add_subplot(rows, cols, i + 1, facecolor="#2d2d2d")
            self._axes.append(ax)
            self._draw(ax, graph_type, i)
            ax.tick_params(colors="#aaaaaa", labelsize=8)
            for spine in ax.spines.values():
                spine.set_edgecolor("#555555")

        self.fig.tight_layout(pad=2.0)
        self.draw()

    def _draw(self, ax, graph_type: str, seed: int):
        rng = np.random.default_rng(seed + np.random.randint(0, 1000))
        colors = ["#4fc3f7", "#81c784", "#ffb74d", "#e57373"]
        c = colors[seed % len(colors)]
        n = 200

        titles = {
            "line":      f"Line Series {seed + 1}",
            "scatter":   f"Scatter Plot {seed + 1}",
            "bar":       f"Bar Chart {seed + 1}",
            "histogram": f"Histogram {seed + 1}",
        }
        ax.set_title(titles.get(graph_type, ""), color="#dddddd", fontsize=9, pad=6)

        if graph_type == "line":
            x = np.linspace(0, 4 * np.pi, n)
            y = rng.normal(0, 0.3, n).cumsum() + np.sin(x + seed)
            ax.plot(x, y, color=c, linewidth=1.2)
            ax.fill_between(x, y, alpha=0.15, color=c)

        elif graph_type == "scatter":
            x = rng.normal(seed, 1.5, n)
            y = rng.normal(seed * 0.5, 1.5, n)
            sizes = rng.uniform(10, 80, n)
            ax.scatter(x, y, c=c, s=sizes, alpha=0.6, edgecolors="none")

        elif graph_type == "bar":
            labels = [f"G{j+1}" for j in range(8)]
            values = rng.uniform(0.5, 5, 8)
            bars = ax.bar(labels, values, color=c, alpha=0.8, width=0.6)
            for bar, val in zip(bars, values):
                ax.text(
                    bar.get_x() + bar.get_width() / 2,
                    bar.get_height() + 0.05,
                    f"{val:.1f}",
                    ha="center", va="bottom", color="#aaaaaa", fontsize=7,
                )

        elif graph_type == "histogram":
            data = rng.normal(loc=seed, scale=1 + seed * 0.3, size=500)
            ax.hist(data, bins=30, color=c, alpha=0.8, edgecolor="#1e1e1e", linewidth=0.4)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Random Graph Viewer")
        self.resize(1000, 720)
        self.setStyleSheet("""
            QMainWindow, QWidget { background-color: #1e1e1e; color: #dddddd; }
            QPushButton {
                background-color: #3c3c3c; color: #dddddd;
                border: 1px solid #555; border-radius: 4px;
                padding: 6px 14px; font-size: 13px;
            }
            QPushButton:hover   { background-color: #505050; }
            QPushButton:pressed { background-color: #2a2a2a; }
            QPushButton[active="true"] {
                background-color: #0d6efd; border-color: #0d6efd;
            }
            QComboBox {
                background-color: #3c3c3c; color: #dddddd;
                border: 1px solid #555; border-radius: 4px;
                padding: 5px 10px; font-size: 13px; min-width: 130px;
            }
            QComboBox QAbstractItemView { background-color: #3c3c3c; color: #dddddd; }
            QLabel { font-size: 13px; }
            QStatusBar { color: #888888; font-size: 11px; }
        """)

        self._graph_count = 4
        self._build_ui()
        self._set_count(4)

    def _build_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        root = QVBoxLayout(central)
        root.setContentsMargins(10, 10, 10, 6)
        root.setSpacing(8)

        # Toolbar row
        toolbar = QHBoxLayout()
        toolbar.setSpacing(10)

        toolbar.addWidget(QLabel("Graph type:"))

        self.combo = QComboBox()
        self.combo.addItems(["line", "scatter", "bar", "histogram"])
        self.combo.currentTextChanged.connect(self._refresh)
        toolbar.addWidget(self.combo)

        toolbar.addWidget(QLabel("Count:"))

        self._count_buttons = {}
        for n in [1, 2, 4, 6, 9]:
            btn = QPushButton(str(n))
            btn.setFixedWidth(40)
            btn.clicked.connect(lambda _, v=n: self._set_count(v))
            toolbar.addWidget(btn)
            self._count_buttons[n] = btn

        btn_refresh = QPushButton("Randomize")
        btn_refresh.clicked.connect(self._refresh)
        toolbar.addWidget(btn_refresh)

        toolbar.addStretch()
        root.addLayout(toolbar)

        # Canvas
        self.canvas = GraphCanvas(self, width=10, height=7)
        root.addWidget(self.canvas, stretch=1)

        # Status bar
        self.status = QStatusBar()
        self.setStatusBar(self.status)

    def _set_count(self, n: int):
        self._graph_count = n
        for val, btn in self._count_buttons.items():
            btn.setProperty("active", val == n)
            btn.style().unpolish(btn)
            btn.style().polish(btn)
        self._refresh()

    def _refresh(self):
        graph_type = self.combo.currentText()
        n = self._graph_count
        self.canvas.plot_random(graph_type, count=n)
        self.status.showMessage(
            f"Showing {n} random {graph_type} graph{'s' if n != 1 else ''}  •  click Randomize to re-roll"
        )


def main():
    app = QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
