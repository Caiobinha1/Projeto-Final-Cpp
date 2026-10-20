import os
from reportlab.lib.pagesizes import letter
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, HRFlowable
)
from reportlab.pdfgen import canvas

class NumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._saved_page_states = []

    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()

    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.draw_page_decorations(num_pages)
            super().showPage()
        super().save()

    def draw_page_decorations(self, page_count):
        self.saveState()
        self.setFont("Helvetica", 8)
        self.setFillColor(colors.HexColor("#555555"))
        
        if self._pageNumber > 1:
            self.drawString(40, 755, "UFSC | CTC | EEL – EEL7323 / EEL510265: Programação C++ para Sistemas Embarcados")
            self.drawRightString(572, 755, "Guia de Implementação Física de Hardware")
            self.setStrokeColor(colors.HexColor("#CCCCCC"))
            self.setLineWidth(0.5)
            self.line(40, 750, 572, 750)
            
        self.setStrokeColor(colors.HexColor("#CCCCCC"))
        self.setLineWidth(0.5)
        self.line(40, 40, 572, 40)
        self.drawString(40, 28, "Smart Parking Controller – Tutorial de Montagem Física (Componentes de Baixo Custo)")
        self.drawRightString(572, 28, f"Página {self._pageNumber} de {page_count}")
        self.restoreState()

def build_pdf(filename="Tutorial_Montagem_Hardware.pdf"):
    doc = SimpleDocTemplate(
        filename,
        pagesize=letter,
        leftMargin=40,
        rightMargin=40,
        topMargin=50,
        bottomMargin=50
    )
    
    styles = getSampleStyleSheet()
    
    c_primary = colors.HexColor("#1A365D")   # Deep Navy
    c_secondary = colors.HexColor("#2B6CB0") # Medium Blue
    c_dark = colors.HexColor("#2D3748")      # Charcoal

    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=15,
        leading=19,
        textColor=c_primary,
        alignment=1,
        spaceAfter=4
    )

    subtitle_style = ParagraphStyle(
        'DocSubtitle',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9.5,
        leading=13,
        textColor=c_secondary,
        alignment=1,
        spaceAfter=10
    )

    h1_style = ParagraphStyle(
        'Heading1_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=11,
        leading=14,
        textColor=c_primary,
        spaceBefore=9,
        spaceAfter=3,
        keepWithNext=True
    )

    body_style = ParagraphStyle(
        'Body_Custom',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8,
        leading=11.5,
        textColor=c_dark,
        spaceAfter=4
    )

    body_bold = ParagraphStyle(
        'Body_Bold',
        parent=body_style,
        fontName='Helvetica-Bold'
    )

    code_style = ParagraphStyle(
        'Code_Custom',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=7,
        leading=9,
        textColor=colors.HexColor("#1A202C")
    )

    story = []

    # Header Banner
    header_data = [
        [
            Paragraph("<b>UNIVERSIDADE FEDERAL DE SANTA CATARINA – UFSC</b><br/>"
                      "Centro Tecnológico (CTC) | Depto. de Engenharia Elétrica e Eletrônica (EEL)<br/>"
                      "<b>Disciplina:</b> EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados<br/>"
                      "<b>Docentes:</b> Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson W. Spengler", 
                      ParagraphStyle('HdrLeft', parent=body_style, fontSize=7.5, leading=10.5, textColor=c_primary)),
            Paragraph("<b>ANEXO DE ENGENHARIA DE HARDWARE</b><br/>"
                      "<b>Projeto:</b> Smart Parking Controller<br/>"
                      "<b>Módulo:</b> Prototipagem Física / Embarcados<br/>"
                      "<b>Semestre:</b> 2026/2", 
                      ParagraphStyle('HdrRight', parent=body_style, fontSize=7.5, leading=10.5, alignment=2, textColor=c_dark))
        ]
    ]
    t_header = Table(header_data, colWidths=[350, 182])
    t_header.setStyle(TableStyle([
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ('LINEBELOW', (0,0), (-1,-1), 1, c_secondary)
    ]))
    story.append(t_header)
    story.append(Spacer(1, 6))

    story.append(Paragraph("GUIA PRÁTICO DE IMPLEMENTAÇÃO FÍSICA: SMART PARKING CONTROLLER", title_style))
    story.append(Paragraph("Especificação de Componentes Acessíveis no Brasil, Diagrama Esquemático e Firmware C++", subtitle_style))
    story.append(HRFlowable(width="100%", thickness=1, color=c_secondary, spaceAfter=6))

    # 1. Obrigatoriedade
    story.append(Paragraph("1. Avaliação sobre a Obrigatoriedade da Montagem Física", h1_style))
    story.append(Paragraph(
        "<b>A montagem do circuito físico NÃO é estritamente obrigatória para aprovação na disciplina.</b> "
        "A especificação oficial do Prof. Dr. Eduardo Augusto Bezerra estabelece textualmente: "
        "<i>'Demonstração do programa funcionando em um microcomputador OU em um sistema embarcado (ou em ambos)'</i>. "
        "Portanto, a solução 100% simulada em software (com a suite de testes C++ e o simulador visual gráfico) cumpre integralmente os requisitos formais. "
        "No entanto, desenvolver a maquete física com Arduino/ESP32 e servo motor agrega enorme valor prático e garante nota máxima na avaliação final.",
        body_style
    ))

    # 2. Lista de Componentes
    story.append(Paragraph("2. Lista de Componentes de Baixo Custo no Mercado Brasileiro", h1_style))
    
    comp_data = [
        [Paragraph("<b>Componente</b>", body_bold), Paragraph("<b>Função no Sistema</b>", body_bold), Paragraph("<b>Faixa de Preço (BR)</b>", body_bold), Paragraph("<b>Disponibilidade</b>", body_bold)],
        [Paragraph("Arduino Nano ou Uno (com cabo)", body_style), Paragraph("Processador embarcado; executa firmware e lê sensores.", body_style), Paragraph("R$ 25,00 a R$ 38,00", body_style), Paragraph("Imediata (Mercado Livre/Shopee)", body_style)],
        [Paragraph("Micro Servo Motor SG90 (9g)", body_style), Paragraph("Atuador mecânico da cancela (haste 0° a 90°).", body_style), Paragraph("R$ 10,00 a R$ 14,00", body_style), Paragraph("Imediata", body_style)],
        [Paragraph("Sensor Ultrassônico HC-SR04", body_style), Paragraph("Sensor de presença de entrada (detecta veículo &lt; 10 cm).", body_style), Paragraph("R$ 9,00 a R$ 12,00", body_style), Paragraph("Imediata", body_style)],
        [Paragraph("Módulo Sensor de Obstáculo IR", body_style), Paragraph("Barreira óptica de travessia na saída da cancela.", body_style), Paragraph("R$ 5,00 a R$ 7,00", body_style), Paragraph("Imediata", body_style)],
        [Paragraph("Botão Push-Button Táctil (6x6mm)", body_style), Paragraph("Solicitação física de tíquete no totem.", body_style), Paragraph("R$ 0,50 a R$ 1,50", body_style), Paragraph("Lojas de eletrônica", body_style)],
        [Paragraph("Display LCD 16x2 com Módulo I2C", body_style), Paragraph("Interface visual do totem (vagas, tíquete, boas-vindas).", body_style), Paragraph("R$ 18,00 a R$ 24,00", body_style), Paragraph("Imediata", body_style)],
        [Paragraph("LEDs (1x Verde, 1x Vermelho) + Resistores", body_style), Paragraph("Semáforo da cancela (Pare / Siga).", body_style), Paragraph("R$ 0,70", body_style), Paragraph("Qualquer loja", body_style)],
        [Paragraph("Buzzer Ativo 5V", body_style), Paragraph("Alerta sonoro de liberação e timeout.", body_style), Paragraph("R$ 2,50 a R$ 4,00", body_style), Paragraph("Lojas maker", body_style)],
        [Paragraph("Mini Protoboard 400 furos + Jumpers", body_style), Paragraph("Interligação de fios sem necessidade de solda.", body_style), Paragraph("R$ 15,00", body_style), Paragraph("Imediata", body_style)]
    ]

    t_comp = Table(comp_data, colWidths=[120, 172, 100, 140])
    t_comp.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#EDF2F7")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#CBD5E0")),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 2),
        ('BOTTOMPADDING', (0,0), (-1,-1), 2),
        ('LEFTPADDING', (0,0), (-1,-1), 3),
        ('RIGHTPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_comp)
    story.append(Spacer(1, 6))

    # 3. Tabela de Ligação
    story.append(Paragraph("3. Esquema de Ligação Elétrica Pino a Pino (Pinout)", h1_style))
    story.append(Paragraph(
        "• <b>Pino D2:</b> Ânodo do LED Vermelho (com resistor de 220Ω em série para o GND);<br/>"
        "• <b>Pino D3:</b> Ânodo do LED Verde (com resistor de 220Ω em série para o GND);<br/>"
        "• <b>Pino D4:</b> Botão do Totem (conecta ao GND, utilizando <code>INPUT_PULLUP</code> interno);<br/>"
        "• <b>Pino D5:</b> Pino OUT do Módulo Infravermelho (Sensor de Travessia);<br/>"
        "• <b>Pino D6:</b> Pino ECHO do Sensor Ultrassônico HC-SR04;<br/>"
        "• <b>Pino D7:</b> Pino TRIGGER do Sensor Ultrassônico HC-SR04;<br/>"
        "• <b>Pino D8:</b> Terminal (+) do Buzzer Ativo;<br/>"
        "• <b>Pino D9 (PWM):</b> Fio Laranja de sinal do Micro Servo Motor SG90;<br/>"
        "• <b>Pinos A4 (SDA) e A5 (SCL):</b> Barramento de dados I2C para o Display LCD 16x2;<br/>"
        "• <b>Alimentação:</b> Linha de 5V e GND do Arduino alimentam diretamente a protoboard via cabo USB.",
        body_style
    ))

    # 4. Firmware e Integração
    story.append(Paragraph("4. Firmware Embarcado e Comunicação USB-Serial", h1_style))
    story.append(Paragraph(
        "O arquivo <code>firmware_arduino_cancela.ino</code> implementa uma Máquina de Estados Finitos local idêntica à do software em C++, "
        "com taxa de amostragem de 20 Hz (intervalos de 50 ms). Além disso, disponibiliza uma interface serial UART em 9600 baud que "
        "permite a comunicação direta com o motor central <code>smart_parking.exe</code> no computador, formando um sistema ciber-físico completo.",
        body_style
    ))

    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"PDF gerado com sucesso: {filename}")

if __name__ == "__main__":
    build_pdf()
