#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Teste de Integracao End-to-End do Protocolo Pipe IPC (--pipe)
Valida a comunicacao real entre Python e o motor C++ (smart_parking.exe),
exercitando modelos com espaco, fila multipla, FSM completa, HAL LCD,
relogio/calendario, polimorfismo tarifario, excecoes e exportacao de arquivos.
"""

import json
import os
import subprocess
import sys


def send_cmd(proc, cmd):
    proc.stdin.write(cmd + "\n")
    proc.stdin.flush()
    return proc.stdout.readline().strip()


def get_status(proc):
    resp = send_cmd(proc, "STATUS")
    assert resp.startswith("JSON_START") and resp.endswith("JSON_END"), f"STATUS invalido: {resp}"
    return json.loads(resp[len("JSON_START") : -len("JSON_END")].strip())


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    exe_name = "smart_parking.exe" if os.name == "nt" else "smart_parking"
    exe_path = os.path.join(root, "sistema_completo", "bin", exe_name)
    assert os.path.exists(exe_path), f"Executavel nao encontrado: {exe_path}"

    proc = subprocess.Popen(
        [exe_path, "--pipe"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text=True,
        bufsize=1,
    )

    try:
        banner = proc.stdout.readline().strip()
        assert banner == "SMART_PARKING_ENGINE_ONLINE", f"Banner inesperado: {banner}"

        # 1. Chegada de 3 veiculos (incluindo modelos com espacos!)
        r = send_cmd(proc, "CHEGAR Carro CAR-101 Toyota Corolla 480 1")
        assert r.startswith("OK"), r
        r = send_cmd(proc, "CHEGAR Moto MOT-102 Yamaha MT07 480 650")
        assert r.startswith("OK"), r
        r = send_cmd(proc, "CHEGAR Caminhao CAM-103 Volvo FH 480 4")
        assert r.startswith("OK"), r

        st = get_status(proc)
        assert st["fila_espera"] == 3, st
        assert st["cancela"] == "VEICULO_DETECTADO", st

        # 2. Emite tiquete do Carro e tenta emitir outro com cancela aberta (deve bloquear)
        r = send_cmd(proc, "EMITIR 480")
        assert r.startswith("OK TIQUETE"), r
        st = get_status(proc)
        assert st["cancela"] == "CANCELA_ABERTA" and st["hal"]["servo"] == 90, st

        r_err = send_cmd(proc, "EMITIR 481")
        assert r_err.startswith("ERRO"), f"Deveria bloquear com cancela aberta: {r_err}"

        # 3. Confirma passagem do Carro -> FSM deve ir automaticamente para VEICULO_DETECTADO (Moto)
        r = send_cmd(proc, "PASSAR")
        assert r.startswith("OK"), r
        st = get_status(proc)
        assert st["cancela"] == "VEICULO_DETECTADO", st
        assert st["fila_espera"] == 2, st

        # 4. Emite Moto, testa TIMEOUT -> ALERTA_BLOQUEIO -> RESET
        r = send_cmd(proc, "EMITIR 490")
        assert r.startswith("OK TIQUETE"), r
        r = send_cmd(proc, "TIMEOUT")
        assert r.startswith("OK"), r
        st = get_status(proc)
        assert st["cancela"] == "ALERTA_BLOQUEIO" and st["hal"]["alarme"] is True, st

        r = send_cmd(proc, "RESET")
        assert r.startswith("OK"), r
        st = get_status(proc)
        assert st["cancela"] == "VEICULO_DETECTADO" and st["hal"]["alarme"] is False, st

        # 5. Emite Caminhao (modelo "Volvo FH", 4 eixos -> 2 vagas contiguas: 3 e 4)
        r = send_cmd(proc, "EMITIR 500")
        assert r.startswith("OK TIQUETE"), r
        r = send_cmd(proc, "PASSAR")
        assert r.startswith("OK"), r

        st = get_status(proc)
        assert st["vagas_ocupadas"] == 4, st
        assert st["vagas"][2]["modelo"] == "Volvo FH", st["vagas"][2]
        assert st["vagas"][3]["placa"] == "CAM-103", st["vagas"][3]

        # 6. Avanca relogio e testa saidas polimorficas
        r = send_cmd(proc, "RELOGIO 630")
        assert r.startswith("OK"), r

        # Carro com tag (480->630 = 150 min -> 16 * 0.9 = 14.40)
        r = send_cmd(proc, "SAIR CAR-101 630")
        assert "14.40" in r, r

        # Moto 650cc (490->630 = 140 min -> 5 + 2*1.5 + 2 = 10.00)
        r = send_cmd(proc, "SAIR MOT-102 630")
        assert "10.00" in r, r

        # Caminhao 4 eixos (500->630 = 130 min -> 15 + 2*5 + 2*2.5 = 30.00)
        r = send_cmd(proc, "SAIR CAM-103 630")
        assert "30.00" in r, r

        st = get_status(proc)
        assert st["vagas_ocupadas"] == 0, st
        assert abs(st["faturamento"] - 54.40) < 1e-6, st
        assert st["categorias"] == {"Carro": 1, "Moto": 1, "Caminhao": 1}, st["categorias"]

        # 7. Testa RECUAR (desistencia)
        r = send_cmd(proc, "CHEGAR Carro DES-999 Onix 640 0")
        assert r.startswith("OK"), r
        r = send_cmd(proc, "RECUAR")
        assert r.startswith("OK"), r
        st = get_status(proc)
        assert st["fila_espera"] == 0 and st["cancela"] == "LIVRE", st

        # 8. Testa exportacao de arquivos CSV/TXT via IPC
        prefix = os.path.join(root, "tests", "_ipc_tmp_report")
        r = send_cmd(proc, f"EXPORTAR {prefix}")
        assert r.startswith("OK"), r
        assert os.path.exists(prefix + ".csv") and os.path.exists(prefix + ".txt")
        os.remove(prefix + ".csv")
        os.remove(prefix + ".txt")

        # 9. Encerramento limpo
        r = send_cmd(proc, "FIM")
        assert r == "BYE", r
        print("[IPC E2E] 100% dos testes de integracao Pipe IPC + JSON + HAL aprovados!")
    finally:
        proc.wait(timeout=5)


if __name__ == "__main__":
    main()
