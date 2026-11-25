#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Smart Parking Controller - Simulador Interativo Pixel-Art (Modo Jogo)
UFSC / CTC / EEL - EEL7323 / EEL510265 - Programacao C++ para Sistemas Embarcados
Autores: Caio Missfeld Carlos e Fabio Augusto Dittrich

Interface grafica 2D com sprites em pixel art (100% Python + Tkinter nativo,
sem dependencias externas) conectada em tempo real via Pipe IPC ao motor C++11
compilado (sistema_completo/bin/smart_parking.exe).

Toda a regra de negocio (FSM da cancela, FilaEncadeada<T>, polimorfismo de
tarifas, heranca multipla RelogioCalendario, HAL, STL, arquivos fstream e
excecoes) e executada nativamente pelo codigo C++.
"""

import json
import math
import os
import random
import subprocess
import tkinter as tk
from tkinter import messagebox, ttk


# ==============================================================================
# GERADOR DE SPRITES EM PIXEL ART (tk.PhotoImage nativo com zoom nearest-neighbor)
# ==============================================================================
def criar_sprite_pixel_art(linhas, paleta, escala=3):
    altura = len(linhas)
    largura = len(linhas[0])
    img = tk.PhotoImage(width=largura, height=altura)
    for y, row in enumerate(linhas):
        for x, ch in enumerate(row):
            cor = paleta.get(ch)
            if cor:
                img.put(cor, (x, y))
    if escala > 1:
        img = img.zoom(escala, escala)
    return img


SPRITE_CARRO_MAPA = [
    "....KKKKKKKK....",
    "...KCCCCCCCCK...",
    "..KCYCCCCCCYCK..",
    "..KCCCCCCCCCCK..",
    ".KWWCCCCCCCCWWK.",
    ".KWWKGGGGGGKWWK.",
    "..KKKGGLLGGKKK..",
    "..KCCGGGGGGCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCGGGGGGCCK..",
    ".KWWKGGGGGGKWWK.",
    ".KWWCCCCCCCCWWK.",
    "..KCCCCCCCCCCK..",
    "..KCRCCCCCCRCK..",
    "...KCCCCCCCCK...",
    "....KKKKKKKK....",
]

SPRITE_CARRO_TAG_MAPA = [
    "....KKKKKKKK....",
    "...KCCCCCCCCK...",
    "..KCYCCCCCCYCK..",
    "..KCCCCCCCCCCK..",
    ".KWWCCCCCCCCWWK.",
    ".KWWKGGTTGGKWWK.",
    "..KKKGGTTGGKKK..",
    "..KCCGGGGGGCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCKBBBBKCCK..",
    "..KCCGGGGGGCCK..",
    ".KWWKGGGGGGKWWK.",
    ".KWWCCCCCCCCWWK.",
    "..KCCCCCCCCCCK..",
    "..KCRCCCCCCRCK..",
    "...KCCCCCCCCK...",
    "....KKKKKKKK....",
]

SPRITE_MOTO_MAPA = [
    "......KKKK......",
    ".....KWWWWK.....",
    "....KWWYYWWK....",
    "..KKKCCCCCCKKK..",
    ".KMMKCCCCCCKMMK.",
    "..KK.KCCCCK.KK..",
    "....KCCBBCCK....",
    "....KCBBBBBK....",
    "....KCBBBBBK....",
    "....KDDDDDDK....",
    "....KDDDDDDK....",
    ".....KDDDDK.....",
    "....KSDDDDSK....",
    ".....KWWWWK.....",
    ".....KWRRWK.....",
    "......KKKK......",
]

# Caminhao largo (horizontal) que ocupa visualmente 2 vagas contiguas
SPRITE_CAMINHAO_MAPA = [
    "..KKKKKKKKKKKKKKKKKKKK........KKKKKKKK....",
    ".KBBBBBBBBBBBBBBBBBBBBK......KCCCCCCCCK...",
    ".KBLBBBBBBBBBBBBBBBBBBK.KKKK.KCCGGGGCCK...",
    ".KBBBBBBBBBBBBBBBBBBBBKKDDDDKKCCGGGGCCK...",
    "KWWBBBBBBBBBBBBBBBBBBBKDDDDDDKCCGGGGCCWWK.",
    "KWWBBBBBBBBBBBBBBBBBBBKDDDDDDKCCCCCCCCWWK.",
    ".KBBBBBBBBBBBBBBBBBBBBKDDDDDDKCCCCCCYCCK..",
    ".KBBBBBBBBBBBBBBBBBBBBKDDDDDDKCCCCCCYCCK..",
    ".KBBBBBBBBBBBBBBBBBBBBKDDDDDDKCCCCCCCCCK..",
    "KWWBBBBBBBBBBBBBBBBBBBKDDDDDDKCCCCCCCCWWK.",
    "KWWBBBBBBBBBBBBBBBBBBBKDDDDDDKCCGGGGCCWWK.",
    ".KBBBBBBBBBBBBBBBBBBBBKKDDDDKKCCGGGGCCK...",
    ".KBRBBBBBBBBBBBBBBBBBBK.KKKK.KCCGGGGCCK...",
    ".KBBBBBBBBBBBBBBBBBBBBK......KCCCCCCCCK...",
    "..KKKKKKKKKKKKKKKKKKKK........KKKKKKKK....",
]

# Caminhao vertical compacto para fila/animacao
SPRITE_CAMINHAO_MINI = [
    "...KKKKKKKKKK...",
    "..KCCCCCCCCCCK..",
    "..KCYCCCCCCYCK..",
    ".KWWKGGGGGGKWWK.",
    ".KWWKGGGGGGKWWK.",
    "..KKKCCCCCCKKK..",
    "....KDDDDDDK....",
    "..KKKKKKKKKKKK..",
    ".KBBBBBBBBBBBBK.",
    "KWWBBBBBBBBBBWWK",
    ".KBBBBBBBBBBBBK.",
    ".KBBBBBBBBBBBBK.",
    "KWWBBBBBBBBBBWWK",
    ".KBBBBBBBBBBBBK.",
    ".KBRBBBBBBBBRBK.",
    "..KKKKKKKKKKKK..",
]

SPRITE_ARBUSTO_MAPA = [
    "....KKKK....",
    "..KKGGGGKK..",
    ".KGGLLGGGGK.",
    "KGGLLLGGGGGK",
    "KGGGGGGDDGGK",
    "KGGGGDDDDGGK",
    ".KGGDDDDGGK.",
    "..KKKKKKKK..",
]

SPRITE_GUARITA_MAPA = [
    ".KKKKKKKKKKKKKK.",
    "KRRRRRRRRRRRRRRK",
    "KRRLLRRRRRRRRRRK",
    "KRRRRRRRRRRRRRRK",
    ".KKKKKKKKKKKKKK.",
    ".KWWWWWWWWWWWWK.",
    ".KWKGGGGGGGGKWK.",
    ".KWKGGLLGGGGKWK.",
    ".KWKGGGGGGGGKWK.",
    ".KWKKKKKKKKKKWK.",
    ".KWWWWWWWWWWWWK.",
    ".KWWWDDDDDDWWWK.",
    ".KWWWDDDDDDWWWK.",
    ".KKKKKKKKKKKKKK.",
]


class SmartParkingGame(tk.Tk):
    def __init__(self, exe_path):
        super().__init__()
        self.title("Smart Parking Tycoon 2D - Simulador Pixel-Art C++ (UFSC EEL7323)")
        self.geometry("1260x820")
        self.minsize(1120, 740)
        self.configure(bg="#0F172A")

        self.exe_path = exe_path
        self.engine_proc = None
        self.hora_simulada = 480  # 08:00
        self.contador_placas = 100
        self.ultimo_dados = None

        # Estado visual / animacoes
        self.angulo_cancela_visual = 0.0
        self.angulo_cancela_alvo = 0.0
        self.frame_tick = 0
        self.modo_auto = False
        self.timer_auto_id = None
        self.veiculos_animando = []  # lista de dicts: {x, y, tx, ty, sprite_key, label}
        self.textos_flutuantes = []  # lista de dicts: {x, y, texto, cor, vida}
        self.meta_extra_por_placa = {}  # placa -> (cor_idx, extra)
        self.vagas_anteriores = {}  # placa -> (vaga_idx, tipo, modelo)
        self.vagas_hitboxes = []  # [(x1, y1, x2, y2, placa)]

        self._carregar_sprites()

        if not self.iniciar_motor_cpp():
            return

        self.criar_interface()
        self._vincular_atalhos()
        self.atualizar_estado_motor(animar=False)
        self.log("Sistema online! Use o painel ou atalhos: [1]Carro [2]Moto [3]Caminhao [Espaco]Emitir/Passar [T]Timeout [R]Reset.")
        self.after(33, self.loop_animacao)

    def _carregar_sprites(self):
        cores_carro = [
            ("#3B82F6", "#1D4ED8"),  # Azul
            ("#EF4444", "#B91C1C"),  # Vermelho
            ("#10B981", "#047857"),  # Esmeralda
            ("#E2E8F0", "#94A3B8"),  # Prata
        ]
        self.sprites = {}
        for idx, (c_main, c_roof) in enumerate(cores_carro):
            pal = {
                "K": "#0F172A",
                "C": c_main,
                "B": c_roof,
                "W": "#1E293B",
                "G": "#7DD3FC",
                "L": "#E0F2FE",
                "Y": "#FDE047",
                "R": "#F87171",
                "T": "#FACC15",
            }
            self.sprites[f"carro_{idx}"] = criar_sprite_pixel_art(SPRITE_CARRO_MAPA, pal, escala=3)
            self.sprites[f"carro_tag_{idx}"] = criar_sprite_pixel_art(SPRITE_CARRO_TAG_MAPA, pal, escala=3)

        pal_moto_std = {
            "K": "#0F172A",
            "C": "#F97316",
            "B": "#C2410C",
            "D": "#334155",
            "W": "#1E293B",
            "M": "#CBD5E1",
            "S": "#94A3B8",
            "Y": "#FEF08A",
            "R": "#EF4444",
        }
        pal_moto_big = dict(pal_moto_std, C="#E11D48", B="#FACC15")
        self.sprites["moto_std"] = criar_sprite_pixel_art(SPRITE_MOTO_MAPA, pal_moto_std, escala=3)
        self.sprites["moto_big"] = criar_sprite_pixel_art(SPRITE_MOTO_MAPA, pal_moto_big, escala=3)

        pal_cam = {
            "K": "#0F172A",
            "C": "#8B5CF6",
            "B": "#C4B5FD",
            "L": "#EDE9FE",
            "D": "#475569",
            "W": "#1E293B",
            "G": "#7DD3FC",
            "Y": "#FDE047",
            "R": "#EF4444",
        }
        self.sprites["caminhao_duplo"] = criar_sprite_pixel_art(SPRITE_CAMINHAO_MAPA, pal_cam, escala=4)
        self.sprites["caminhao_mini"] = criar_sprite_pixel_art(SPRITE_CAMINHAO_MINI, pal_cam, escala=3)

        pal_arb = {"K": "#052E16", "G": "#16A34A", "L": "#4ADE80", "D": "#15803D"}
        self.sprites["arbusto"] = criar_sprite_pixel_art(SPRITE_ARBUSTO_MAPA, pal_arb, escala=3)

        pal_gua = {
            "K": "#0F172A",
            "R": "#DC2626",
            "L": "#FCA5A5",
            "W": "#E2E8F0",
            "G": "#38BDF8",
            "D": "#64748B",
        }
        self.sprites["guarita"] = criar_sprite_pixel_art(SPRITE_GUARITA_MAPA, pal_gua, escala=4)

    # ==========================================================================
    # COMUNICACAO IPC COM O BINARIO C++
    # ==========================================================================
    def iniciar_motor_cpp(self):
        try:
            popen_kwargs = dict(
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                text=True,
                bufsize=1,
            )
            if os.name == "nt" and hasattr(subprocess, "CREATE_NO_WINDOW"):
                popen_kwargs["creationflags"] = subprocess.CREATE_NO_WINDOW

            self.engine_proc = subprocess.Popen(
                [self.exe_path, "--pipe"],
                **popen_kwargs,
            )
            banner = self.engine_proc.stdout.readline().strip()
            if banner != "SMART_PARKING_ENGINE_ONLINE":
                raise RuntimeError(
                    f"Resposta inesperada do motor C++: '{banner}' (codigo: {self.engine_proc.poll()})"
                )
            return True
        except Exception as e:
            messagebox.showerror(
                "Erro ao Iniciar Motor C++",
                f"Nao foi possivel iniciar o executavel C++ em:\n{self.exe_path}\n\nErro: {e}\n\n"
                "Compile o projeto com 'make sistema' ou via g++ antes de abrir o simulador.",
            )
            self.destroy()
            return False

    def enviar_comando_cpp(self, comando):
        if not self.engine_proc or self.engine_proc.poll() is not None:
            self.log("[FALHA] Motor C++ desconectado.")
            return ""
        try:
            self.engine_proc.stdin.write(comando + "\n")
            self.engine_proc.stdin.flush()
            return self.engine_proc.stdout.readline().strip()
        except Exception as e:
            self.log(f"[ERRO IPC]: {e}")
            return ""

    def obter_status_cpp(self):
        resp = self.enviar_comando_cpp("STATUS")
        if resp.startswith("JSON_START") and resp.endswith("JSON_END"):
            json_str = resp[len("JSON_START") : -len("JSON_END")].strip()
            try:
                return json.loads(json_str)
            except Exception as e:
                self.log(f"[ERRO JSON]: {e}")
        return None

    # ==========================================================================
    # INTERFACE GRAFICA
    # ==========================================================================
    def criar_interface(self):
        # Barra superior (HUD)
        top_frame = tk.Frame(self, bg="#1E293B", height=54)
        top_frame.pack(fill=tk.X, side=tk.TOP)

        tk.Label(
            top_frame,
            text="SMART PARKING TYCOON 2D  |  MOTOR C++11 EM TEMPO REAL (UFSC EEL7323)",
            font=("Consolas", 12, "bold"),
            fg="#38BDF8",
            bg="#1E293B",
        ).pack(side=tk.LEFT, padx=14, pady=10)

        self.btn_auto = tk.Button(
            top_frame,
            text="▶ Modo Jogo Auto: OFF",
            bg="#334155",
            fg="#F8FAFC",
            font=("Consolas", 9, "bold"),
            relief=tk.FLAT,
            padx=10,
            command=self.alternar_modo_auto,
        )
        self.btn_auto.pack(side=tk.RIGHT, padx=10, pady=8)

        self.lbl_clock = tk.Label(
            top_frame,
            text="24/11/2026 08:00",
            font=("Consolas", 12, "bold"),
            fg="#FACC15",
            bg="#0F172A",
            padx=10,
            pady=4,
        )
        self.lbl_clock.pack(side=tk.RIGHT, padx=6, pady=8)

        # Corpo principal
        main_container = tk.Frame(self, bg="#0F172A")
        main_container.pack(fill=tk.BOTH, expand=True, padx=10, pady=8)

        # Painel Lateral de Controle (Esquerda com Scroll leve via layout compacto)
        ctrl_frame = tk.LabelFrame(
            main_container,
            text=" Controles do Simulador (C++11) ",
            font=("Consolas", 9, "bold"),
            fg="#93C5FD",
            bg="#1E293B",
            width=295,
        )
        ctrl_frame.pack(side=tk.LEFT, fill=tk.Y, padx=(0, 10))
        ctrl_frame.pack_propagate(False)

        # 1. Chegada de Veiculos (Polimorfismo)
        tk.Label(
            ctrl_frame,
            text="1. Chegada na Cancela (Polimorfismo):",
            font=("Helvetica", 8, "bold"),
            fg="#CBD5E1",
            bg="#1E293B",
        ).pack(anchor="w", padx=8, pady=(6, 2))

        row_car = tk.Frame(ctrl_frame, bg="#1E293B")
        row_car.pack(fill=tk.X, padx=8, pady=2)
        tk.Button(
            row_car,
            text="Carro",
            bg="#2563EB",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=lambda: self.simular_chegada("Carro", 0),
            width=14,
        ).pack(side=tk.LEFT, padx=(0, 3))
        tk.Button(
            row_car,
            text="Carro c/ Tag (-10%)",
            bg="#1D4ED8",
            fg="#FDE047",
            font=("Helvetica", 8, "bold"),
            command=lambda: self.simular_chegada("Carro", 1),
            width=16,
        ).pack(side=tk.RIGHT)

        row_moto = tk.Frame(ctrl_frame, bg="#1E293B")
        row_moto.pack(fill=tk.X, padx=8, pady=2)
        tk.Button(
            row_moto,
            text="Moto 250cc",
            bg="#EA580C",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=lambda: self.simular_chegada("Moto", 250),
            width=14,
        ).pack(side=tk.LEFT, padx=(0, 3))
        tk.Button(
            row_moto,
            text="Moto 650cc (+R$2)",
            bg="#C2410C",
            fg="#FEF08A",
            font=("Helvetica", 8, "bold"),
            command=lambda: self.simular_chegada("Moto", 650),
            width=16,
        ).pack(side=tk.RIGHT)

        row_cam = tk.Frame(ctrl_frame, bg="#1E293B")
        row_cam.pack(fill=tk.X, padx=8, pady=2)
        tk.Button(
            row_cam,
            text="Caminhao 2e",
            bg="#7C3AED",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=lambda: self.simular_chegada("Caminhao", 2),
            width=14,
        ).pack(side=tk.LEFT, padx=(0, 3))
        tk.Button(
            row_cam,
            text="Caminhao 4e (2 vag)",
            bg="#6D28D9",
            fg="#EDE9FE",
            font=("Helvetica", 8, "bold"),
            command=lambda: self.simular_chegada("Caminhao", 4),
            width=16,
        ).pack(side=tk.RIGHT)

        # 2. Maquina de Estados (FSM)
        tk.Label(
            ctrl_frame,
            text="2. Operacoes da Cancela (FSM):",
            font=("Helvetica", 8, "bold"),
            fg="#CBD5E1",
            bg="#1E293B",
        ).pack(anchor="w", padx=8, pady=(8, 2))

        tk.Button(
            ctrl_frame,
            text="Emitir Tiquete & Abrir Cancela [Espaco]",
            bg="#16A34A",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=self.emitir_tiquete,
        ).pack(fill=tk.X, padx=8, pady=2)

        tk.Button(
            ctrl_frame,
            text="Cruzar Cancela & Fechar [Enter]",
            bg="#0D9488",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=self.cruzar_cancela,
        ).pack(fill=tk.X, padx=8, pady=2)

        row_fsm = tk.Frame(ctrl_frame, bg="#1E293B")
        row_fsm.pack(fill=tk.X, padx=8, pady=2)
        tk.Button(
            row_fsm,
            text="Timeout [T]",
            bg="#DC2626",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=self.simular_timeout,
            width=10,
        ).pack(side=tk.LEFT, padx=(0, 2))
        tk.Button(
            row_fsm,
            text="Reset [R]",
            bg="#2563EB",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=self.resetar_alerta,
            width=9,
        ).pack(side=tk.LEFT, padx=2)
        tk.Button(
            row_fsm,
            text="Recuar",
            bg="#64748B",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=self.recuar_veiculo,
            width=9,
        ).pack(side=tk.RIGHT)

        # 3. Pagamento e Saida
        tk.Label(
            ctrl_frame,
            text="3. Saida & Pagamento (Clique na vaga ou lista):",
            font=("Helvetica", 8, "bold"),
            fg="#CBD5E1",
            bg="#1E293B",
        ).pack(anchor="w", padx=8, pady=(8, 2))

        self.cbo_veiculos = ttk.Combobox(ctrl_frame, state="readonly")
        self.cbo_veiculos.pack(fill=tk.X, padx=8, pady=2)

        tk.Button(
            ctrl_frame,
            text="Registrar Saida e Cobrar Tarifa",
            bg="#D97706",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=self.registrar_saida,
        ).pack(fill=tk.X, padx=8, pady=2)

        # 4. RelogioCalendario (Heranca Multipla)
        tk.Label(
            ctrl_frame,
            text="4. RelogioCalendario (Heranca Multipla):",
            font=("Helvetica", 8, "bold"),
            fg="#CBD5E1",
            bg="#1E293B",
        ).pack(anchor="w", padx=8, pady=(8, 2))

        row_clk = tk.Frame(ctrl_frame, bg="#1E293B")
        row_clk.pack(fill=tk.X, padx=8, pady=2)
        for lbl, mins in [("+15m", 15), ("+30m", 30), ("+1h", 60), ("+12h", 720)]:
            tk.Button(
                row_clk,
                text=lbl,
                bg="#475569",
                fg="white",
                font=("Consolas", 8, "bold"),
                command=lambda m=mins: self.avancar_relogio(m),
                width=6,
            ).pack(side=tk.LEFT, expand=True, fill=tk.X, padx=1)

        # 5. Testes Rapidos de Excecoes & Arquivos
        tk.Label(
            ctrl_frame,
            text="5. Demonstracao (Excecoes & Arquivos):",
            font=("Helvetica", 8, "bold"),
            fg="#CBD5E1",
            bg="#1E293B",
        ).pack(anchor="w", padx=8, pady=(8, 2))

        row_exc = tk.Frame(ctrl_frame, bg="#1E293B")
        row_exc.pack(fill=tk.X, padx=8, pady=2)
        tk.Button(
            row_exc,
            text="Testar Duplicada",
            bg="#7F1D1D",
            fg="#FECACA",
            font=("Helvetica", 7, "bold"),
            command=self.testar_excecao_duplicada,
            width=15,
        ).pack(side=tk.LEFT, padx=(0, 2))
        tk.Button(
            row_exc,
            text="Placa Inexistente",
            bg="#7F1D1D",
            fg="#FECACA",
            font=("Helvetica", 7, "bold"),
            command=self.testar_excecao_inexistente,
            width=15,
        ).pack(side=tk.RIGHT)

        tk.Button(
            ctrl_frame,
            text="Exportar Relatorio CSV & TXT (<fstream>)",
            bg="#0F766E",
            fg="white",
            font=("Helvetica", 8, "bold"),
            command=self.exportar_relatorio,
        ).pack(fill=tk.X, padx=8, pady=3)

        # 6. Indicadores STL / FSM em Tempo Real
        kpi_box = tk.Frame(ctrl_frame, bg="#0F172A", highlightbackground="#334155", highlightthickness=1)
        kpi_box.pack(fill=tk.X, padx=8, pady=(6, 6))

        self.lbl_vagas_info = tk.Label(
            kpi_box, text="Vagas: 10 Livres / 0 Ocupadas", font=("Consolas", 8, "bold"), fg="#4ADE80", bg="#0F172A"
        )
        self.lbl_vagas_info.pack(anchor="w", padx=6, pady=(4, 1))

        self.lbl_fila_info = tk.Label(
            kpi_box, text="Fila Template: 0 | FSM: LIVRE", font=("Consolas", 8), fg="#FDE047", bg="#0F172A"
        )
        self.lbl_fila_info.pack(anchor="w", padx=6, pady=1)

        self.lbl_fat_info = tk.Label(
            kpi_box, text="Faturamento: R$ 0.00 (Med: 0.00)", font=("Consolas", 8, "bold"), fg="#38BDF8", bg="#0F172A"
        )
        self.lbl_fat_info.pack(anchor="w", padx=6, pady=1)

        self.lbl_cat_info = tk.Label(
            kpi_box, text="STL Map: 0 Car | 0 Mot | 0 Cam", font=("Consolas", 8), fg="#E2E8F0", bg="#0F172A"
        )
        self.lbl_cat_info.pack(anchor="w", padx=6, pady=(1, 4))

        # Canvas Central (Mapa Pixel-Art do Estacionamento)
        center_frame = tk.Frame(main_container, bg="#0F172A")
        center_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        self.canvas = tk.Canvas(
            center_frame, bg="#1E293B", highlightthickness=2, highlightbackground="#334155", cursor="hand2"
        )
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.canvas.bind("<Button-1>", self.on_canvas_click)
        self.canvas.bind("<Double-Button-1>", self.on_canvas_double_click)

        # Terminal IPC C++ (Rodape)
        log_frame = tk.LabelFrame(
            self,
            text=" Terminal IPC em Tempo Real (Python <-> smart_parking.exe --pipe) ",
            font=("Consolas", 8, "bold"),
            fg="#94A3B8",
            bg="#0F172A",
            height=115,
        )
        log_frame.pack(fill=tk.X, side=tk.BOTTOM, padx=10, pady=(0, 8))

        self.txt_log = tk.Text(
            log_frame, height=5, bg="#020617", fg="#38BDF8", font=("Consolas", 8), wrap="word"
        )
        self.txt_log.pack(fill=tk.BOTH, expand=True, padx=4, pady=4)

    def _vincular_atalhos(self):
        self.bind("1", lambda _e: self.simular_chegada("Carro", random.choice([0, 1])))
        self.bind("2", lambda _e: self.simular_chegada("Moto", random.choice([250, 650])))
        self.bind("3", lambda _e: self.simular_chegada("Caminhao", random.choice([2, 3, 4])))
        self.bind("<space>", self._atalho_espaco)
        self.bind("<Return>", lambda _e: self.cruzar_cancela())
        self.bind("t", lambda _e: self.simular_timeout())
        self.bind("T", lambda _e: self.simular_timeout())
        self.bind("r", lambda _e: self.resetar_alerta())
        self.bind("R", lambda _e: self.resetar_alerta())
        self.bind("<plus>", lambda _e: self.avancar_relogio(30))
        self.bind("<KP_Add>", lambda _e: self.avancar_relogio(30))

    def _atalho_espaco(self, _evt):
        if self.ultimo_dados and self.ultimo_dados.get("cancela") == "CANCELA_ABERTA":
            self.cruzar_cancela()
        else:
            self.emitir_tiquete()

    def log(self, mensagem):
        h = (self.hora_simulada // 60) % 24
        m = self.hora_simulada % 60
        self.txt_log.insert(tk.END, f"[{h:02d}:{m:02d}] {mensagem}\n")
        self.txt_log.see(tk.END)

    # ==========================================================================
    # ACOES DO USUARIO & MODO JOGO AUTOMATICO
    # ==========================================================================
    def alternar_modo_auto(self):
        self.modo_auto = not self.modo_auto
        if self.modo_auto:
            self.btn_auto.config(text="⏸ Modo Jogo Auto: ON", bg="#16A34A")
            self.log("[MODO JOGO] Trafego automatico ativado! Novos veiculos chegarao periodicamente.")
            self._agendar_trafego_auto()
        else:
            self.btn_auto.config(text="▶ Modo Jogo Auto: OFF", bg="#334155")
            if self.timer_auto_id:
                self.after_cancel(self.timer_auto_id)
                self.timer_auto_id = None
            self.log("[MODO JOGO] Trafego automatico pausado.")

    def _agendar_trafego_auto(self):
        if not self.modo_auto:
            return
        if self.ultimo_dados and self.ultimo_dados.get("fila_espera", 0) < 4:
            escolha = random.choices(["Carro", "Moto", "Caminhao"], weights=[55, 30, 15])[0]
            if escolha == "Carro":
                extra = random.choice([0, 1])
            elif escolha == "Moto":
                extra = random.choice([250, 650])
            else:
                extra = random.choice([2, 3, 4])
            self.simular_chegada(escolha, extra)
            self.avancar_relogio(10)
        self.timer_auto_id = self.after(4000, self._agendar_trafego_auto)

    def avancar_relogio(self, minutos):
        self.hora_simulada += minutos
        resp = self.enviar_comando_cpp(f"RELOGIO {self.hora_simulada}")
        self.log(f"[C++ CMD] RELOGIO {self.hora_simulada} -> {resp}")
        self.atualizar_estado_motor(animar=False)

    def simular_chegada(self, tipo, extra=0):
        self.contador_placas += 1
        placa = f"{tipo[:3].upper()}-{self.contador_placas}"
        modelos = {
            "Carro": ["Toyota Corolla", "Honda Civic", "VW Polo", "GM Onix"],
            "Moto": ["Honda CB500", "Yamaha MT07", "Fazer 250"],
            "Caminhao": ["Volvo FH", "Scania R450", "Mercedes Actros"],
        }
        modelo = random.choice(modelos[tipo])
        self.meta_extra_por_placa[placa] = (self.contador_placas % 4, extra)

        cmd = f"CHEGAR {tipo} {placa} {modelo} {self.hora_simulada} {extra}"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[C++ CMD] {cmd} -> {resp}")
        self.atualizar_estado_motor(animar=False)

    def emitir_tiquete(self):
        cmd = f"EMITIR {self.hora_simulada}"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[C++ CMD] {cmd} -> {resp}")
        if resp.startswith("ERRO"):
            messagebox.showwarning("Excecao C++ Capturada", resp)
        self.atualizar_estado_motor(animar=True)

    def cruzar_cancela(self):
        resp = self.enviar_comando_cpp("PASSAR")
        self.log(f"[C++ CMD] PASSAR -> {resp}")
        if resp.startswith("ERRO"):
            messagebox.showwarning("FSM Cancela", resp)
        self.atualizar_estado_motor(animar=False)

    def simular_timeout(self):
        resp = self.enviar_comando_cpp("TIMEOUT")
        self.log(f"[C++ CMD] TIMEOUT -> {resp}")
        if resp.startswith("ERRO"):
            messagebox.showwarning("FSM Timeout", resp)
        self.atualizar_estado_motor(animar=False)

    def resetar_alerta(self):
        resp = self.enviar_comando_cpp("RESET")
        self.log(f"[C++ CMD] RESET -> {resp}")
        if resp.startswith("ERRO"):
            messagebox.showwarning("FSM Reset", resp)
        self.atualizar_estado_motor(animar=False)

    def recuar_veiculo(self):
        resp = self.enviar_comando_cpp("RECUAR")
        self.log(f"[C++ CMD] RECUAR -> {resp}")
        if resp.startswith("OK"):
            self._criar_texto_flutuante(115, 290, "RECUSADO / RECUOU", "#FCA5A5")
        else:
            messagebox.showwarning("FSM Recuo", resp)
        self.atualizar_estado_motor(animar=False)

    def registrar_saida(self, placa_forcada=None):
        if placa_forcada:
            placa = placa_forcada
        else:
            sel = self.cbo_veiculos.get()
            if not sel:
                messagebox.showwarning("Aviso", "Selecione um veiculo estacionado (ou clique na vaga).")
                return
            placa = sel.split(" - ")[0].strip()

        pos_vaga = self._obter_centro_placa(placa)
        cmd = f"SAIR {placa} {self.hora_simulada}"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[C++ CMD] {cmd} -> {resp}")
        if resp.startswith("OK SAIDA"):
            valor = resp.split(" ")[-1]
            if pos_vaga:
                txt = "ISENTO (<=15m)" if float(valor) == 0.0 else f"+R$ {valor}"
                cor = "#4ADE80" if float(valor) == 0.0 else "#FACC15"
                self._criar_texto_flutuante(pos_vaga[0], pos_vaga[1], txt, cor)
            self.atualizar_estado_motor(animar=False)
        else:
            messagebox.showerror("Erro de Saida (Excecao C++)", resp)

    def testar_excecao_duplicada(self):
        # Tenta inserir uma placa que ja existe no patio ou cria uma e duplica
        alvo = None
        if self.ultimo_dados:
            for v in self.ultimo_dados.get("vagas", []):
                if v.get("ocupada"):
                    alvo = v.get("placa")
                    break
        if not alvo:
            alvo = "DUP-900"
            self.enviar_comando_cpp(f"CHEGAR Carro {alvo} VW Polo {self.hora_simulada} 0")
        cmd = f"CHEGAR Carro {alvo} VW Polo {self.hora_simulada} 0"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[TESTE EXCECAO] {cmd} -> {resp}")
        messagebox.showwarning("PlacaDuplicadaException (C++)", resp)
        self.atualizar_estado_motor(animar=False)

    def testar_excecao_inexistente(self):
        cmd = f"SAIR XXX-0000 {self.hora_simulada}"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[TESTE EXCECAO] {cmd} -> {resp}")
        messagebox.showerror("TiqueteInvalidoException (C++)", resp)

    def exportar_relatorio(self):
        resp = self.enviar_comando_cpp("EXPORTAR relatorio_smart_parking")
        self.log(f"[C++ CMD] EXPORTAR -> {resp}")
        if resp.startswith("OK"):
            messagebox.showinfo("Arquivos Gerados (<fstream>)", resp)
        else:
            messagebox.showerror("Erro de Arquivo", resp)

    # ==========================================================================
    # CLIQUE DIRETO NAS VAGAS DO PATIO
    # ==========================================================================
    def _placa_em_coordenada(self, x, y):
        for x1, y1, x2, y2, placa in self.vagas_hitboxes:
            if x1 <= x <= x2 and y1 <= y <= y2:
                return placa
        return None

    def on_canvas_click(self, event):
        placa = self._placa_em_coordenada(event.x, event.y)
        if not placa:
            return
        for idx, item in enumerate(self.cbo_veiculos["values"]):
            if item.startswith(placa + " - "):
                self.cbo_veiculos.current(idx)
                self.log(f"[SELECAO] Veiculo {placa} selecionado (duplo clique na vaga para cobrar e sair).")
                break

    def on_canvas_double_click(self, event):
        placa = self._placa_em_coordenada(event.x, event.y)
        if placa:
            self.registrar_saida(placa_forcada=placa)

    # ==========================================================================
    # ATUALIZACAO DE ESTADO E ANIMACAO 30 FPS
    # ==========================================================================
    def _obter_retangulo_vaga(self, idx, w, h):
        largura_vaga = max(110, (w - 240) // 5)
        altura_vaga = 138
        fila_idx = 0 if idx < 5 else 1
        col_idx = idx if idx < 5 else idx - 5
        vx1 = 215 + col_idx * largura_vaga
        vx2 = vx1 + largura_vaga - 12
        if fila_idx == 0:
            vy1 = 26
            vy2 = vy1 + altura_vaga
        else:
            vy1 = h - 26 - altura_vaga
            vy2 = vy1 + altura_vaga
        return vx1, vy1, vx2, vy2

    def _obter_centro_placa(self, placa):
        for x1, y1, x2, y2, p in self.vagas_hitboxes:
            if p == placa:
                return ((x1 + x2) // 2, (y1 + y2) // 2)
        return None

    def _criar_texto_flutuante(self, x, y, texto, cor):
        self.textos_flutuantes.append({"x": x, "y": y, "texto": texto, "cor": cor, "vida": 45})

    def _sprite_para_veiculo(self, placa, tipo):
        cor_idx, extra = self.meta_extra_por_placa.get(placa, (hash(placa) % 4, 0))
        if tipo == "Carro":
            return f"carro_tag_{cor_idx}" if extra == 1 else f"carro_{cor_idx}"
        if tipo == "Moto":
            return "moto_big" if extra > 500 else "moto_std"
        return "caminhao_mini"

    def atualizar_estado_motor(self, animar=False):
        dados = self.obter_status_cpp()
        if not dados:
            return
        self.ultimo_dados = dados

        ocup = dados.get("vagas_ocupadas", 0)
        disp = dados.get("vagas_disponiveis", 10)
        fila = dados.get("fila_espera", 0)
        fat = dados.get("faturamento", 0.0)
        t_med = dados.get("ticket_medio", 0.0)
        cancela = dados.get("cancela", "LIVRE")
        data_hora = dados.get("data_hora", "24/11/2026 08:00")
        cats = dados.get("categorias", {})
        hal = dados.get("hal", {})

        self.angulo_cancela_alvo = float(hal.get("servo", 0))

        self.lbl_clock.config(text=f" {data_hora} ")
        self.lbl_vagas_info.config(text=f"Vagas: {disp} Livres / {ocup} Ocupadas")
        self.lbl_fila_info.config(text=f"Fila: {fila} | FSM: {cancela}")
        self.lbl_fat_info.config(text=f"Fat: R$ {fat:.2f} (Med: R$ {t_med:.2f})")
        self.lbl_cat_info.config(
            text=f"STL Map: {cats.get('Carro',0)} Car | {cats.get('Moto',0)} Mot | {cats.get('Caminhao',0)} Cam"
        )

        # Deduplica combobox (caminhoes ocupam 2 vagas)
        vagas_por_placa = {}
        meta_por_placa = {}
        for v in dados.get("vagas", []):
            if v.get("ocupada"):
                p = v.get("placa")
                vagas_por_placa.setdefault(p, []).append(v.get("vaga"))
                meta_por_placa[p] = (v.get("tipo"), v.get("modelo"))

        # Dispara animacao de entrada para novos veiculos recem-alocados
        if animar:
            w = max(840, self.canvas.winfo_width())
            h = max(560, self.canvas.winfo_height())
            for p, nums in vagas_por_placa.items():
                if p not in self.vagas_anteriores:
                    vx1, vy1, vx2, vy2 = self._obter_retangulo_vaga(nums[0] - 1, w, h)
                    tx = (vx1 + vx2) // 2
                    ty = (vy1 + vy2) // 2
                    tipo = meta_por_placa[p][0]
                    self.veiculos_animando.append(
                        {
                            "x": 140.0,
                            "y": float(h // 2),
                            "tx": float(tx),
                            "ty": float(ty),
                            "sprite": self._sprite_para_veiculo(p, tipo),
                            "placa": p,
                        }
                    )
        self.vagas_anteriores = {p: meta_por_placa[p] for p in vagas_por_placa}

        lista_combo = []
        for p, nums in vagas_por_placa.items():
            t, m = meta_por_placa[p]
            vaga_txt = f"Vagas {nums[0]}-{nums[-1]}" if len(nums) > 1 else f"Vaga {nums[0]}"
            lista_combo.append(f"{p} - {t} {m} ({vaga_txt})")

        atual = self.cbo_veiculos.get()
        self.cbo_veiculos["values"] = lista_combo
        if lista_combo:
            if atual not in lista_combo:
                self.cbo_veiculos.current(0)
        else:
            self.cbo_veiculos.set("")

    def loop_animacao(self):
        self.frame_tick += 1

        # Suaviza movimento do braco da cancela (0 a 90 graus)
        diff = self.angulo_cancela_alvo - self.angulo_cancela_visual
        if abs(diff) > 0.5:
            self.angulo_cancela_visual += diff * 0.28
        else:
            self.angulo_cancela_visual = self.angulo_cancela_alvo

        # Atualiza veiculos dirigindo para a vaga
        ativos = []
        for anim in self.veiculos_animando:
            dx = anim["tx"] - anim["x"]
            dy = anim["ty"] - anim["y"]
            dist = math.hypot(dx, dy)
            if dist > 12:
                vel = 14.0
                anim["x"] += (dx / dist) * vel
                anim["y"] += (dy / dist) * vel
                ativos.append(anim)
        self.veiculos_animando = ativos

        # Atualiza textos flutuantes (+R$ ...)
        novos_txt = []
        for t in self.textos_flutuantes:
            t["y"] -= 1.2
            t["vida"] -= 1
            if t["vida"] > 0:
                novos_txt.append(t)
        self.textos_flutuantes = novos_txt

        if self.ultimo_dados:
            self.desenhar_cena(self.ultimo_dados)

        self.after(33, self.loop_animacao)

    # ==========================================================================
    # RENDERIZACAO 2D PIXEL-ART NO CANVAS
    # ==========================================================================
    def desenhar_cena(self, dados):
        c = self.canvas
        c.delete("all")
        self.vagas_hitboxes = []

        w = c.winfo_width()
        h = c.winfo_height()
        if w < 200:
            w = 880
        if h < 200:
            h = 580

        hal = dados.get("hal", {})
        cancela = dados.get("cancela", "LIVRE")
        fila = dados.get("fila_espera", 0)
        alarme = hal.get("alarme", False)
        led_verde = hal.get("led_verde", False)
        lcd1 = hal.get("lcd1", "CANCELA PRONTA")
        lcd2 = hal.get("lcd2", "VAGAS: 10 LIVRES")

        # 1. Fundo: Grama lateral + Calcada + Asfalto
        c.create_rectangle(0, 0, w, h, fill="#14532D", outline="")
        c.create_rectangle(10, 10, w - 10, h - 10, fill="#334155", outline="#64748B", width=3)
        c.create_rectangle(20, 18, w - 20, h - 18, fill="#1E293B", outline="")

        # Arbustos pixel-art decorativos nos cantos da grama
        for ax, ay in [(188, 22), (188, h - 22), (w - 22, h // 2 - 62), (w - 22, h // 2 + 62)]:
            c.create_image(ax, ay, image=self.sprites["arbusto"])

        # 2. Pista central asfaltada e faixas de rolamento
        pista_y1 = h // 2 - 54
        pista_y2 = h // 2 + 54
        c.create_rectangle(20, pista_y1, w - 20, pista_y2, fill="#0F172A", outline="#475569", width=2)
        for x in range(185, w - 30, 44):
            c.create_rectangle(x, h // 2 - 3, x + 22, h // 2 + 3, fill="#FACC15", outline="")
        c.create_text(w - 64, h // 2 - 32, text="SAIDA ->", fill="#94A3B8", font=("Consolas", 8, "bold"))

        # 3. Display LCD 16x2 Virtual (HAL C++) no topo esquerdo
        c.create_rectangle(18, 20, 182, 98, fill="#022C22", outline="#34D399", width=2)
        c.create_rectangle(24, 38, 176, 86, fill="#064E3B", outline="#10B981", width=1)
        c.create_text(100, 29, text="HAL LCD 16x2 (C++)", fill="#6EE7B7", font=("Consolas", 7, "bold"))
        c.create_text(100, 52, text=lcd1, fill="#A7F3D0", font=("Consolas", 9, "bold"))
        c.create_text(100, 72, text=lcd2, fill="#A7F3D0", font=("Consolas", 9))

        # 4. Guarita Pixel-Art + Sirene de Alerta + Semaforo
        c.create_image(68, h // 2 - 12, image=self.sprites["guarita"])
        c.create_text(68, h // 2 + 36, text=f"SERVO: {int(self.angulo_cancela_alvo)}°", fill="#FACC15", font=("Consolas", 8, "bold"))

        if alarme:
            piscar = (self.frame_tick // 6) % 2 == 0
            cor_sirene = "#EF4444" if piscar else "#FACC15"
            c.create_oval(56, h // 2 - 58, 80, h // 2 - 34, fill=cor_sirene, outline="white", width=2)
            c.create_text(68, h // 2 - 68, text="!! ALERTA BUZZER !!", fill="#F87171", font=("Consolas", 8, "bold"))

        # Semaforo ao lado da cancela
        c.create_rectangle(106, h // 2 - 52, 126, h // 2 - 12, fill="#020617", outline="#94A3B8")
        c.create_oval(
            110, h // 2 - 48, 122, h // 2 - 36,
            fill="#EF4444" if not led_verde else "#450A0A", outline=""
        )
        c.create_oval(
            110, h // 2 - 30, 122, h // 2 - 18,
            fill="#22C55E" if led_verde else "#052E16", outline=""
        )

        # 5. Cancela listrada animada (rotacao suave 0° -> 90°)
        piv_x, piv_y = 148, h // 2 + 44
        comprimento = 88
        rad = math.radians(self.angulo_cancela_visual)
        # 0 deg = bloqueando verticalmente a pista (para cima); 90 deg = levantada/recolhida na horizontal fora da pista
        # Para vista top-down estilo arcade: 0 deg = atravessando a pista (x=148, y=pista_y1..pista_y2); 90 deg = aberta para a direita
        fim_x = piv_x + comprimento * math.sin(rad)
        fim_y = piv_y - comprimento * math.cos(rad)
        c.create_line(piv_x, piv_y, fim_x, fim_y, fill="#EF4444", width=8, capstyle=tk.ROUND)
        c.create_line(piv_x, piv_y, fim_x, fim_y, fill="#F8FAFC", width=4, dash=(8, 8))
        c.create_oval(piv_x - 7, piv_y - 7, piv_x + 7, piv_y + 7, fill="#F59E0B", outline="#0F172A", width=2)

        cor_st = "#4ADE80" if cancela == "CANCELA_ABERTA" else ("#F87171" if cancela == "ALERTA_BLOQUEIO" else "#FDE047")
        c.create_text(148, pista_y1 - 14, text=f"FSM: {cancela}", fill=cor_st, font=("Consolas", 8, "bold"))

        # 6. Fila externa de espera (FilaEncadeada<Veiculo*>)
        c.create_text(98, h - 145, text=f"FILA EXTERNA: {fila}", fill="#FDE047", font=("Consolas", 8, "bold"))
        for f in range(min(fila, 3)):
            fx = 45 + f * 52
            fy = h - 92
            c.create_image(fx, fy, image=self.sprites[f"carro_{f % 4}"])
            c.create_text(fx, fy + 38, text=f"#{f+1}", fill="#CBD5E1", font=("Consolas", 7, "bold"))

        # 7. Desenho das 10 Vagas com Sprites Pixel-Art (incluindo Caminhao de 2 vagas!)
        vagas = dados.get("vagas", [])
        placas_animando = {a["placa"] for a in self.veiculos_animando}
        vagas_puladas = set()

        for i in range(10):
            if i in vagas_puladas:
                continue
            vx1, vy1, vx2, vy2 = self._obter_retangulo_vaga(i, w, h)
            info = vagas[i] if i < len(vagas) else {"ocupada": False}
            ocupada = info.get("ocupada", False)

            # Detecta caminhao ocupando 2 vagas contiguas na mesma fileira (i e i+1)
            eh_dupla = False
            if ocupada and i not in (4, 9) and (i + 1) < len(vagas):
                prox = vagas[i + 1]
                if prox.get("ocupada") and prox.get("placa") == info.get("placa"):
                    eh_dupla = True
                    vagas_puladas.add(i + 1)
                    _, _, vx2, _ = self._obter_retangulo_vaga(i + 1, w, h)

            cx = (vx1 + vx2) // 2
            cy = (vy1 + vy2) // 2

            if not ocupada:
                c.create_rectangle(vx1, vy1, vx2, vy2, fill="#0F291E", outline="#10B981", width=2)
                # Linhas de balizamento da vaga
                c.create_text(cx, vy1 + 16, text=f"VAGA {i+1}", fill="#6EE7B7", font=("Consolas", 9, "bold"))
                c.create_text(cx, cy + 6, text="[ LIVRE ]", fill="#34D399", font=("Consolas", 8))
            else:
                tipo = info.get("tipo", "Carro")
                placa = info.get("placa", "---")
                modelo = info.get("modelo", "")
                entrada_min = info.get("entrada", 480)
                tempo = max(0, self.hora_simulada - entrada_min)

                cor_bg = "#172554" if tipo == "Carro" else ("#431407" if tipo == "Moto" else "#2E1065")
                cor_bd = "#60A5FA" if tipo == "Carro" else ("#FB923C" if tipo == "Moto" else "#C084FC")
                c.create_rectangle(vx1, vy1, vx2, vy2, fill=cor_bg, outline=cor_bd, width=2)

                vaga_lbl = f"VAGAS {i+1}-{i+2}" if eh_dupla else f"VAGA {i+1}"
                c.create_text(cx, vy1 + 11, text=f"{vaga_lbl} | {placa}", fill="#FDE047", font=("Consolas", 8, "bold"))

                if placa not in placas_animando:
                    if eh_dupla:
                        c.create_image(cx, cy + 4, image=self.sprites["caminhao_duplo"])
                    else:
                        spr_key = self._sprite_para_veiculo(placa, tipo)
                        c.create_image(cx, cy + 2, image=self.sprites[spr_key])

                c.create_text(cx, vy2 - 12, text=f"{modelo[:15]} ({tempo}m)", fill="#E2E8F0", font=("Consolas", 7))
                self.vagas_hitboxes.append((vx1, vy1, vx2, vy2, placa))

        # 8. Sprites em transito (dirigindo da cancela ate a vaga)
        for anim in self.veiculos_animando:
            c.create_image(int(anim["x"]), int(anim["y"]), image=self.sprites[anim["sprite"]])
            c.create_text(
                int(anim["x"]), int(anim["y"]) - 36, text=anim["placa"], fill="#FDE047", font=("Consolas", 8, "bold")
            )

        # 9. Textos flutuantes (+R$ 16.00 / ISENTO)
        for t in self.textos_flutuantes:
            c.create_text(int(t["x"]), int(t["y"]), text=t["texto"], fill=t["cor"], font=("Consolas", 11, "bold"))

    def destroy(self):
        self.modo_auto = False
        if self.timer_auto_id:
            try:
                self.after_cancel(self.timer_auto_id)
            except Exception:
                pass
        if self.engine_proc and self.engine_proc.poll() is None:
            try:
                self.engine_proc.stdin.write("FIM\n")
                self.engine_proc.stdin.flush()
                self.engine_proc.terminate()
            except Exception:
                pass
        super().destroy()


if __name__ == "__main__":
    diretorio_atual = os.path.dirname(os.path.abspath(__file__))
    exe_name = "smart_parking.exe" if os.name == "nt" else "smart_parking"
    caminho_exe = os.path.join(diretorio_atual, "sistema_completo", "bin", exe_name)

    if not os.path.exists(caminho_exe):
        caminho_exe = os.path.abspath(os.path.join("sistema_completo", "bin", exe_name))

    app = SmartParkingGame(caminho_exe)
    if app.engine_proc is not None:
        app.mainloop()
