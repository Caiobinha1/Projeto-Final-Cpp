#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Smart Parking Controller - Simulador Gráfico Interativo & Testador Visual
Disciplina: EEL7323 / EEL510265 - Programação C++ para Sistemas Embarcados (UFSC)
Professores: Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson W. Spengler

Este programa fornece uma interface gráfica (GUI) interativa estilo "jogo de simulação",
que se conecta diretamente via Pipe IPC com o binário C++ compilado (smart_parking.exe).
Toda a lógica de negócios, máquina de estados da cancela (FSM), estruturas de dados e
tarifação polimórfica são executadas nativamente pelo código C++ do projeto.
"""

import os
import sys
import json
import random
import subprocess
import tkinter as tk
from tkinter import ttk, messagebox

class SmartParkingSimulator(tk.Tk):
    def __init__(self, exe_path):
        super().__init__()
        self.title("Smart Parking Tycoon - Simulador Visual C++ (UFSC EEL7323)")
        self.geometry("1100x750")
        self.minsize(980, 680)
        self.configure(bg="#1A202C")

        self.exe_path = exe_path
        self.engine_proc = None
        self.hora_simulada = 480 # 08:00 em minutos (8 * 60)
        self.contador_placas = 100

        self.iniciar_motor_cpp()
        self.criar_interface()
        self.atualizar_estado_motor()

    def iniciar_motor_cpp(self):
        try:
            self.engine_proc = subprocess.Popen(
                [self.exe_path, "--pipe"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                text=True,
                bufsize=1
            )
            # Lê linha de boas-vindas do C++
            linha_inicio = self.engine_proc.stdout.readline().strip()
            print("[C++ Engine Conectado]:", linha_inicio)
        except Exception as e:
            messagebox.showerror(
                "Erro ao Iniciar Motor C++",
                f"Não foi possível iniciar o executável C++ em:\n{self.exe_path}\n\nErro: {e}\n\n"
                "Certifique-se de compilar o binário em EntregaFinal_Sistema_Completo/bin/smart_parking.exe."
            )
            self.destroy()

    def enviar_comando_cpp(self, comando):
        if not self.engine_proc or self.engine_proc.poll() is not None:
            self.log("[FALHA] Motor C++ desconectado.")
            return ""

        try:
            self.engine_proc.stdin.write(comando + "\n")
            self.engine_proc.stdin.flush()
            resposta = self.engine_proc.stdout.readline().strip()
            return resposta
        except Exception as e:
            self.log(f"[ERRO IPC]: {e}")
            return ""

    def obter_status_cpp(self):
        resp = self.enviar_comando_cpp("STATUS")
        if resp.startswith("JSON_START") and resp.endswith("JSON_END"):
            json_str = resp[len("JSON_START"): -len("JSON_END")].strip()
            try:
                return json.loads(json_str)
            except Exception as e:
                self.log(f"[ERRO PARSE JSON]: {e}")
        return None

    def criar_interface(self):
        # Top Header
        top_frame = tk.Frame(self, bg="#2D3748", height=60)
        top_frame.pack(fill=tk.X, side=tk.TOP, padx=0, pady=0)

        lbl_title = tk.Label(
            top_frame,
            text="SMART PARKING CONTROLLER – SIMULADOR VISUAL C++",
            font=("Helvetica", 14, "bold"),
            fg="#63B3ED",
            bg="#2D3748"
        )
        lbl_title.pack(side=tk.LEFT, padx=15, pady=10)

        self.lbl_clock = tk.Label(
            top_frame,
            text="Horário: 08:00",
            font=("Courier", 13, "bold"),
            fg="#F6E05E",
            bg="#2D3748"
        )
        self.lbl_clock.pack(side=tk.RIGHT, padx=20, pady=10)

        # Main Layout: Esquerda (Controles) | Direita (Pátio Visual e Logs)
        main_container = tk.Frame(self, bg="#1A202C")
        main_container.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Painel Lateral de Controle (Esquerda)
        ctrl_frame = tk.LabelFrame(main_container, text=" Painel de Controle ", font=("Helvetica", 10, "bold"), fg="#E2E8F0", bg="#2D3748", width=250)
        ctrl_frame.pack(side=tk.LEFT, fill=tk.Y, padx=(0, 10), pady=0)

        # Botões de Chegada
        lbl_chegada = tk.Label(ctrl_frame, text="1. Simular Chegada na Cancela:", font=("Helvetica", 9, "bold"), fg="#CBD5E0", bg="#2D3748")
        lbl_chegada.pack(anchor="w", padx=10, pady=(10, 4))

        btn_carro = tk.Button(ctrl_frame, text="🚗 Chegar Carro", bg="#3182CE", fg="white", font=("Helvetica", 9, "bold"), command=lambda: self.simular_chegada("Carro"), width=22)
        btn_carro.pack(padx=10, pady=3)

        btn_moto = tk.Button(ctrl_frame, text="🏍️ Chegar Moto", bg="#DD6B20", fg="white", font=("Helvetica", 9, "bold"), command=lambda: self.simular_chegada("Moto"), width=22)
        btn_moto.pack(padx=10, pady=3)

        btn_cam = tk.Button(ctrl_frame, text="🚛 Chegar Caminhão", bg="#805AD5", fg="white", font=("Helvetica", 9, "bold"), command=lambda: self.simular_chegada("Caminhao"), width=22)
        btn_cam.pack(padx=10, pady=3)

        # Ações da Cancela
        lbl_cancela = tk.Label(ctrl_frame, text="2. Operações da Cancela:", font=("Helvetica", 9, "bold"), fg="#CBD5E0", bg="#2D3748")
        lbl_cancela.pack(anchor="w", padx=10, pady=(15, 4))

        self.btn_emitir = tk.Button(ctrl_frame, text="🎫 Pressionar Botão / Tíquete", bg="#38A169", fg="white", font=("Helvetica", 9, "bold"), command=self.emitir_tiquete, width=22)
        self.btn_emitir.pack(padx=10, pady=3)

        self.btn_passar = tk.Button(ctrl_frame, text="🚙 Cruzar Cancela (Fechar)", bg="#4A5568", fg="white", font=("Helvetica", 9, "bold"), command=self.cruzar_cancela, width=22)
        self.btn_passar.pack(padx=10, pady=3)

        # Saída de Veículo
        lbl_saida = tk.Label(ctrl_frame, text="3. Pagamento e Saída:", font=("Helvetica", 9, "bold"), fg="#CBD5E0", bg="#2D3748")
        lbl_saida.pack(anchor="w", padx=10, pady=(15, 4))

        self.cbo_veiculos = ttk.Combobox(ctrl_frame, state="readonly", width=20)
        self.cbo_veiculos.pack(padx=10, pady=3)

        btn_pagar = tk.Button(ctrl_frame, text="💰 Registrar Saída e Pagar", bg="#D69E2E", fg="white", font=("Helvetica", 9, "bold"), command=self.registrar_saida, width=22)
        btn_pagar.pack(padx=10, pady=3)

        # Avançar Tempo
        lbl_tempo = tk.Label(ctrl_frame, text="4. Controle do Relógio:", font=("Helvetica", 9, "bold"), fg="#CBD5E0", bg="#2D3748")
        lbl_tempo.pack(anchor="w", padx=10, pady=(15, 4))

        btn_t15 = tk.Button(ctrl_frame, text="⏱️ +15 min", bg="#4A5568", fg="white", font=("Helvetica", 8), command=lambda: self.avancar_relogio(15), width=22)
        btn_t15.pack(padx=10, pady=2)

        btn_t30 = tk.Button(ctrl_frame, text="⏱️ +30 min", bg="#4A5568", fg="white", font=("Helvetica", 8), command=lambda: self.avancar_relogio(30), width=22)
        btn_t30.pack(padx=10, pady=2)

        btn_t60 = tk.Button(ctrl_frame, text="⏱️ +1 Hora", bg="#4A5568", fg="white", font=("Helvetica", 8), command=lambda: self.avancar_relogio(60), width=22)
        btn_t60.pack(padx=10, pady=2)

        # KPIs do Sistema
        lbl_kpi = tk.Label(ctrl_frame, text="5. Indicadores em Tempo Real:", font=("Helvetica", 9, "bold"), fg="#CBD5E0", bg="#2D3748")
        lbl_kpi.pack(anchor="w", padx=10, pady=(15, 4))

        self.lbl_vagas_info = tk.Label(ctrl_frame, text="Vagas: 10 Livres", font=("Helvetica", 9), fg="#68D391", bg="#2D3748")
        self.lbl_vagas_info.pack(anchor="w", padx=12, pady=1)

        self.lbl_fila_info = tk.Label(ctrl_frame, text="Fila Entrada: 0", font=("Helvetica", 9), fg="#FBD38D", bg="#2D3748")
        self.lbl_fila_info.pack(anchor="w", padx=12, pady=1)

        self.lbl_fat_info = tk.Label(ctrl_frame, text="Faturamento: R$ 0,00", font=("Helvetica", 9, "bold"), fg="#63B3ED", bg="#2D3748")
        self.lbl_fat_info.pack(anchor="w", padx=12, pady=1)

        # Painel Central (Visual do Pátio e Cancela)
        center_frame = tk.Frame(main_container, bg="#1A202C")
        center_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        self.canvas = tk.Canvas(center_frame, bg="#2D3748", highlightthickness=1, highlightbackground="#4A5568")
        self.canvas.pack(fill=tk.BOTH, expand=True)

        # Terminal de Logs C++ (Abaixo)
        log_frame = tk.LabelFrame(self, text=" Terminal de Comunicação em Tempo Real com o Motor C++ (smart_parking.exe) ", font=("Helvetica", 8, "bold"), fg="#A0AEC0", bg="#1A202C", height=120)
        log_frame.pack(fill=tk.X, side=tk.BOTTOM, padx=10, pady=(0, 10))

        self.txt_log = tk.Text(log_frame, height=5, bg="#0F172A", fg="#38BDF8", font=("Consolas", 8), wrap="word")
        self.txt_log.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

    def log(self, mensagem):
        hora_str = f"{self.hora_simulada // 60:02d}:{self.hora_simulada % 60:02d}"
        self.txt_log.insert(tk.END, f"[{hora_str}] {mensagem}\n")
        self.txt_log.see(tk.END)

    def avancar_relogio(self, minutos):
        self.hora_simulada += minutos
        h = self.hora_simulada // 60
        m = self.hora_simulada % 60
        self.lbl_clock.config(text=f"Horário: {h:02d}:{m:02d}")
        self.log(f"Relógio avançou +{minutos} min -> Agora são {h:02d}:{m:02d}.")
        self.atualizar_estado_motor()

    def simular_chegada(self, tipo):
        self.contador_placas += 1
        placa = f"{tipo[:3].upper()}-{self.contador_placas}"
        modelos = {
            "Carro": ["Corolla", "Civic", "Polo", "Onix"],
            "Moto": ["CB500", "Yamaha MT07", "Fazer250"],
            "Caminhao": ["Volvo FH", "Scania R450", "Mercedes Actros"]
        }
        modelo = random.choice(modelos[tipo])
        extra = 0
        if tipo == "Carro":
            extra = 1 if random.random() > 0.5 else 0
        elif tipo == "Moto":
            extra = random.choice([250, 500, 650])
        elif tipo == "Caminhao":
            extra = random.choice([2, 3, 4])

        cmd = f"CHEGAR {tipo} {placa} {modelo} {self.hora_simulada} {extra}"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[C++ CMD]: {cmd} -> {resp}")
        self.atualizar_estado_motor()

    def emitir_tiquete(self):
        cmd = f"EMITIR {self.hora_simulada}"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[C++ CMD]: {cmd} -> {resp}")
        if resp.startswith("OK TIQUETE"):
            messagebox.showinfo("Tíquete Emitido!", f"{resp}\n\nA cancela abriu! Clique em 'Cruzar Cancela' para concluir a entrada.")
        elif resp.startswith("ERRO"):
            messagebox.showwarning("Aviso do Sistema", resp)
        self.atualizar_estado_motor()

    def cruzar_cancela(self):
        resp = self.enviar_comando_cpp("PASSAR")
        self.log(f"[C++ CMD]: PASSAR -> {resp}")
        self.atualizar_estado_motor()

    def registrar_saida(self):
        sel = self.cbo_veiculos.get()
        if not sel:
            messagebox.showwarning("Aviso", "Selecione um veículo estacionado para registrar saída.")
            return

        placa = sel.split(" - ")[0].strip()
        cmd = f"SAIR {placa} {self.hora_simulada}"
        resp = self.enviar_comando_cpp(cmd)
        self.log(f"[C++ CMD]: {cmd} -> {resp}")
        if resp.startswith("OK SAIDA"):
            partes = resp.split(" ")
            valor = partes[-1]
            messagebox.showinfo("Comprovante de Saída", f"Veículo {placa} liberado com sucesso!\n\nValor Pago (Cálculo Polimórfico C++): R$ {valor}")
        elif resp.startswith("ERRO"):
            messagebox.showerror("Erro de Saída", resp)
        self.atualizar_estado_motor()

    def atualizar_estado_motor(self):
        dados = self.obter_status_cpp()
        if not dados:
            return

        # Atualiza KPIs
        ocup = dados.get("vagas_ocupadas", 0)
        disp = dados.get("vagas_disponiveis", 10)
        fila = dados.get("fila_espera", 0)
        fat = dados.get("faturamento", 0.0)
        cancela_estado = dados.get("cancela", "LIVRE")

        self.lbl_vagas_info.config(text=f"Vagas: {disp} Livres / {ocup} Ocupadas")
        self.lbl_fila_info.config(text=f"Fila na Cancela: {fila} veículo(s)")
        self.lbl_fat_info.config(text=f"Faturamento: R$ {fat:.2f}")

        # Atualiza combobox de veículos para saída
        lista_combo = []
        for v in dados.get("vagas", []):
            if v.get("ocupada"):
                p = v.get("placa")
                t = v.get("tipo")
                m = v.get("modelo")
                num = v.get("vaga")
                item = f"{p} - {t} {m} (Vaga {num})"
                if item not in lista_combo:
                    lista_combo.append(item)
        self.cbo_veiculos["values"] = lista_combo
        if lista_combo and not self.cbo_veiculos.get():
            self.cbo_veiculos.current(0)
        elif not lista_combo:
            self.cbo_veiculos.set("")

        # Redesenha Pátio Visual no Canvas
        self.desenhar_patio(dados, cancela_estado, fila)

    def desenhar_patio(self, dados, cancela_estado, fila):
        self.canvas.delete("all")
        w = self.canvas.winfo_width()
        h = self.canvas.winfo_height()
        if w < 100: w = 700
        if h < 100: h = 480

        # Asfalto da pista central
        self.canvas.create_rectangle(150, h//2 - 40, w - 20, h//2 + 40, fill="#1E293B", outline="")

        # Faixa tracejada central
        for x in range(160, w - 20, 40):
            self.canvas.create_line(x, h//2, x + 20, h//2, fill="#E2E8F0", width=2)

        # Guarita e Cancela (Esquerda)
        # Guarita
        self.canvas.create_rectangle(20, h//2 - 50, 80, h//2 + 50, fill="#475569", outline="#94A3B8", width=2)
        self.canvas.create_text(50, h//2, text="GUARITA\nCONTROLE", fill="white", font=("Helvetica", 7, "bold"), justify="center")

        # Semáforo da cancela
        cor_luz = "#22C55E" if cancela_estado == "CANCELA_ABERTA" else "#EF4444"
        self.canvas.create_oval(90, h//2 - 35, 106, h//2 - 19, fill=cor_luz, outline="white")

        # Braço da cancela
        if cancela_estado == "CANCELA_ABERTA":
            # Cancela levantada
            self.canvas.create_line(110, h//2, 110, h//2 - 60, fill="#F59E0B", width=6)
            self.canvas.create_text(110, h//2 - 70, text="ABERTA", fill="#22C55E", font=("Helvetica", 8, "bold"))
        else:
            # Cancela abaixada
            self.canvas.create_line(110, h//2 - 30, 110, h//2 + 30, fill="#EF4444", width=6)
            self.canvas.create_text(110, h//2 - 40, text="FECHADA", fill="#EF4444", font=("Helvetica", 8, "bold"))

        # Fila de espera antes da cancela
        if fila > 0:
            self.canvas.create_text(50, h//2 + 70, text=f"FILA: {fila} aguardando", fill="#FBD38D", font=("Helvetica", 9, "bold"))
            for f in range(min(fila, 3)):
                self.canvas.create_rectangle(20, h//2 + 90 + f*25, 80, h//2 + 110 + f*25, fill="#38BDF8", outline="white")
                self.canvas.create_text(50, h//2 + 100 + f*25, text="VEÍCULO", fill="black", font=("Helvetica", 7, "bold"))

        # Desenho das 10 Vagas (5 no topo, 5 embaixo)
        vagas = dados.get("vagas", [])
        largura_vaga = (w - 200) // 5
        altura_vaga = 90

        for i in range(10):
            fila_idx = 0 if i < 5 else 1
            col_idx = i if i < 5 else i - 5

            vx1 = 170 + col_idx * largura_vaga
            vx2 = vx1 + largura_vaga - 15

            if fila_idx == 0:
                vy1 = 20
                vy2 = vy1 + altura_vaga
            else:
                vy1 = h - 20 - altura_vaga
                vy2 = vy1 + altura_vaga

            info_vaga = vagas[i] if i < len(vagas) else {"ocupada": False}
            ocupada = info_vaga.get("ocupada", False)

            if not ocupada:
                # Vaga Livre (Verde Escuro)
                self.canvas.create_rectangle(vx1, vy1, vx2, vy2, fill="#064E3B", outline="#10B981", width=2)
                self.canvas.create_text((vx1 + vx2)//2, (vy1 + vy2)//2 - 10, text=f"VAGA {i+1}", fill="#6EE7B7", font=("Helvetica", 9, "bold"))
                self.canvas.create_text((vx1 + vx2)//2, (vy1 + vy2)//2 + 10, text="[ LIVRE ]", fill="#A7F3D0", font=("Helvetica", 8))
            else:
                tipo = info_vaga.get("tipo", "Veículo")
                placa = info_vaga.get("placa", "---")
                modelo = info_vaga.get("modelo", "")
                entrada_min = info_vaga.get("entrada", 480)
                tempo_decorrido = max(0, self.hora_simulada - entrada_min)

                cor_box = "#1E3A8A" if tipo == "Carro" else ("#7C2D12" if tipo == "Moto" else "#581C87")
                cor_borda = "#60A5FA" if tipo == "Carro" else ("#FB923C" if tipo == "Moto" else "#C084FC")
                icone = "🚗" if tipo == "Carro" else ("🏍️" if tipo == "Moto" else "🚛")

                self.canvas.create_rectangle(vx1, vy1, vx2, vy2, fill=cor_box, outline=cor_borda, width=2)
                self.canvas.create_text((vx1 + vx2)//2, vy1 + 16, text=f"VAGA {i+1} - {icone} {tipo}", fill="white", font=("Helvetica", 8, "bold"))
                self.canvas.create_text((vx1 + vx2)//2, vy1 + 34, text=f"{placa}", fill="#FDE047", font=("Helvetica", 9, "bold"))
                self.canvas.create_text((vx1 + vx2)//2, vy1 + 52, text=f"{modelo}", fill="#E2E8F0", font=("Helvetica", 7))
                self.canvas.create_text((vx1 + vx2)//2, vy1 + 72, text=f"Tempo: {tempo_decorrido} min", fill="#93C5FD", font=("Helvetica", 7, "bold"))

    def destroy(self):
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
    caminho_exe = os.path.join(diretorio_atual, "EntregaFinal_Sistema_Completo", "bin", "smart_parking.exe")

    if not os.path.exists(caminho_exe):
        # Tenta procurar a partir da pasta raiz
        caminho_exe = os.path.abspath("EntregaFinal_Sistema_Completo/bin/smart_parking.exe")

    app = SmartParkingSimulator(caminho_exe)
    app.mainloop()
