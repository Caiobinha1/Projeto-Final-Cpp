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
        
        # Header (pages > 1)
        if self._pageNumber > 1:
            self.drawString(40, 755, "UFSC | CTC | EEL – EEL7323: Programação C++ para Sistemas Embarcados")
            self.drawRightString(572, 755, "Proposta de Projeto Final – Smart Parking")
            self.setStrokeColor(colors.HexColor("#CCCCCC"))
            self.setLineWidth(0.5)
            self.line(40, 750, 572, 750)
            
        # Footer (all pages)
        self.setStrokeColor(colors.HexColor("#CCCCCC"))
        self.setLineWidth(0.5)
        self.line(40, 40, 572, 40)
        self.drawString(40, 28, "Sistema de Controle de Estacionamento Inteligente (Smart Parking Controller)")
        self.drawRightString(572, 28, f"Página {self._pageNumber} de {page_count}")
        self.restoreState()

def build_pdf(filename="Proposta_Projeto_Estacionamento.pdf"):
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
    c_bg_box = colors.HexColor("#F7FAFC")

    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=16,
        leading=20,
        textColor=c_primary,
        alignment=1,
        spaceAfter=4
    )

    subtitle_style = ParagraphStyle(
        'DocSubtitle',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=10.5,
        leading=14,
        textColor=c_secondary,
        alignment=1,
        spaceAfter=12
    )

    h1_style = ParagraphStyle(
        'Heading1_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=12,
        leading=16,
        textColor=c_primary,
        spaceBefore=12,
        spaceAfter=5,
        keepWithNext=True
    )

    h2_style = ParagraphStyle(
        'Heading2_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=10,
        leading=13,
        textColor=c_secondary,
        spaceBefore=7,
        spaceAfter=3,
        keepWithNext=True
    )

    body_style = ParagraphStyle(
        'Body_Custom',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=12,
        textColor=c_dark,
        spaceAfter=5
    )

    body_bold = ParagraphStyle(
        'Body_Bold',
        parent=body_style,
        fontName='Helvetica-Bold'
    )

    story = []

    # Header Banner
    header_data = [
        [
            Paragraph("<b>UNIVERSIDADE FEDERAL DE SANTA CATARINA – UFSC</b><br/>"
                      "Centro Tecnológico (CTC) | Depto. de Engenharia Elétrica e Eletrônica (EEL)<br/>"
                      "<b>Disciplina:</b> EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados<br/>"
                      "<b>Docentes:</b> Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson W. Spengler", 
                      ParagraphStyle('HdrLeft', parent=body_style, fontSize=8, leading=11, textColor=c_primary)),
            Paragraph("<b>PROPOSTA DE PROJETO FINAL</b><br/>"
                      "<b>Semestre:</b> 2026/2<br/>"
                      "<b>Modalidade:</b> Individual / Dupla<br/>"
                      "<b>Status:</b> Em Definição", 
                      ParagraphStyle('HdrRight', parent=body_style, fontSize=8, leading=11, alignment=2, textColor=c_dark))
        ]
    ]
    t_header = Table(header_data, colWidths=[350, 182])
    t_header.setStyle(TableStyle([
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('BOTTOMPADDING', (0,0), (-1,-1), 5),
        ('LINEBELOW', (0,0), (-1,-1), 1, c_secondary)
    ]))
    story.append(t_header)
    story.append(Spacer(1, 8))

    story.append(Paragraph("PROPOSTA DE DESENVOLVIMENTO: SMART PARKING CONTROLLER", title_style))
    story.append(Paragraph("Sistema de Controle de Estacionamento Inteligente Baseado em Máquina de Estados, Polimorfismo Dinâmico e Estruturas de Dados Encadeadas", subtitle_style))
    story.append(HRFlowable(width="100%", thickness=1, color=c_secondary, spaceAfter=8))

    # 1. Visão Geral e Justificativa
    story.append(Paragraph("1. Visão Geral do Projeto e Justificativa de Engenharia", h1_style))
    story.append(Paragraph(
        "O projeto proposto consiste no desenvolvimento de um <b>Controlador de Estacionamento Inteligente (Smart Parking Controller)</b>, "
        "projetado sob a ótica de sistemas embarcados em tempo real. O sistema gerencia a detecção de veículos na entrada, emissão e validação "
        "de tíquetes, controle da cancela automatizada através de uma Máquina de Estados Finitos (FSM), alocação de vagas dinâmicas e cálculo tarifário diferenciado.",
        body_style
    ))
    story.append(Paragraph(
        "<b>Por que este projeto é ideal para a disciplina?</b> Na programação procedural clássica (como C puro), sistemas desse tipo tornam-se frágeis, "
        "repletos de estruturas <code>switch/case</code> extensas para diferenciar tipos de veículos e de ponteiros genéricos desprotegidos (<code>void*</code>). "
        "O domínio do estacionamento permite demonstrar a superioridade da <b>Orientação a Objetos em C++</b> ao substituir tais estruturas por polimorfismo dinâmico, "
        "encapsulamento estrito e estruturas de dados dinâmicas genéricas com alocação determinística (RAII).",
        body_style
    ))

    # 2. Conceitos de OO
    story.append(Paragraph("2. Mapeamento dos Conceitos de Orientação a Objetos em C++", h1_style))
    
    oo_data = [
        [Paragraph("<b>Conceito C++ Exigido</b>", body_bold), Paragraph("<b>Aplicação Concreta no Projeto</b>", body_bold), Paragraph("<b>Benefício Técnico / Justificativa</b>", body_bold)],
        [
            Paragraph("<b>Classes Abstratas e Polimorfismo</b>", body_style),
            Paragraph("Classe base <code>Veiculo</code> com métodos virtuais puros <code>calcularTarifa(tempo) = 0</code> e <code>getTipo() = 0</code>. Classes derivadas: <code>Carro</code>, <code>Moto</code> e <code>Caminhao</code>.", body_style),
            Paragraph("Princípio Aberto/Fechado (OCP): o controlador opera sobre ponteiros <code>Veiculo*</code> via <i>vtable</i>, permitindo adicionar novos veículos sem alterar o código existente.", body_style)
        ],
        [
            Paragraph("<b>Destrutor Virtual e RAII</b>", body_style),
            Paragraph("Destrutor <code>virtual ~Veiculo() = default;</code> na classe base.", body_style),
            Paragraph("Garante a liberação polimórfica correta de memória no momento em que uma vaga é desocupada (<code>delete vagas[i]</code>), evitando vazamentos de memória (memory leaks).", body_style)
        ],
        [
            Paragraph("<b>Máquina de Estados (FSM)</b>", body_style),
            Paragraph("Classe <code>FSM_Controlador</code> modelando estados: <i>LIVRE, VEICULO_DETECTADO, EMITINDO_TIQUETE, CANCELA_ABERTA, BLOQUEIO_ALERTA</i>.", body_style),
            Paragraph("Controle determinístico e seguro do atuador da cancela, impedindo transições ilegais e colisões físicas.", body_style)
        ],
        [
            Paragraph("<b>Templates (Generics)</b>", body_style),
            Paragraph("Classe template <code>FilaEncadeada&lt;T&gt;</code> e estrutura de nó genérico <code>Nodo&lt;T&gt;</code>.", body_style),
            Paragraph("Reutilização de código com total checagem estática de tipos (type safety) para enfileirar tanto requisições de entrada quanto logs de saída.", body_style)
        ],
        [
            Paragraph("<b>Estrutura Dinâmica Própria</b>", body_style),
            Paragraph("Fila encadeada implementada manualmente com gerenciamento explícito de ponteiros (<code>new</code>/<code>delete</code>).", body_style),
            Paragraph("Atende a exigência de demonstrar domínio sobre estruturas dinâmicas em sistemas com recursos restritos, sem dependência exclusiva da STL.", body_style)
        ],
        [
            Paragraph("<b>Friends e Sobrecarga de Operadores</b>", body_style),
            Paragraph("Sobrecarga de <code>operator&lt;&lt;</code> com <code>friend</code> para formatação de tíquetes e cupons fiscais.", body_style),
            Paragraph("Acesso direto e seguro a membros protegidos para serialização legível em tela e arquivo, mantendo o encapsulamento perante o resto do software.", body_style)
        ],
        [
            Paragraph("<b>Tratamento de Exceções</b>", body_style),
            Paragraph("Classes derivadas de <code>std::exception</code>: <code>EstacionamentoLotadoException</code>, <code>FilaVaziaException</code>, etc.", body_style),
            Paragraph("Robustez no tratamento de condições anômalas sem necessidade de códigos de erro numéricos vulneráveis.", body_style)
        ]
    ]

    t_oo = Table(oo_data, colWidths=[110, 230, 192])
    t_oo.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#EDF2F7")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#CBD5E0")),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
        ('LEFTPADDING', (0,0), (-1,-1), 4),
        ('RIGHTPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_oo)
    story.append(Spacer(1, 8))

    # 3. Requisitos do Projeto
    story.append(Paragraph("3. Especificação de Requisitos", h1_style))
    story.append(Paragraph("<b>3.1 Requisitos Funcionais (RF):</b>", h2_style))
    story.append(Paragraph(
        "• <b>RF01 – Detecção e Entrada:</b> Detectar veículo na entrada e enfileirá-lo na cancela de acesso.<br/>"
        "• <b>RF02 – Emissão de Tíquete:</b> Gerar tíquete com identificador único, carimbo de data/hora simulada e placa.<br/>"
        "• <b>RF03 – Controle da Cancela:</b> Abrir a cancela após emissão de tíquete e fechá-la após confirmação de passagem pelo sensor de laço indutivo.<br/>"
        "• <b>RF04 – Alocação de Vagas:</b> Alocar veículos em vetor polimórfico de vagas (motos ocupam vagas reduzidas, caminhões vagas especiais).<br/>"
        "• <b>RF05 – Cálculo Tarifário Polimórfico:</b> Calcular valor devido com base na classe do veículo e tempo de permanência.<br/>"
        "• <b>RF06 – Registro e Faturamento:</b> Armazenar histórico de saídas e total faturado.",
        body_style
    ))
    story.append(Paragraph("<b>3.2 Requisitos Não-Funcionais (RNF):</b>", h2_style))
    story.append(Paragraph(
        "• <b>RNF01 – Padrão de Linguagem:</b> C++ moderno (C++11) compilável com GCC/g++ sob Scientific Linux / MinGW.<br/>"
        "• <b>RNF02 – Gerenciamento de Memória:</b> Zero vazamentos de memória (verificável com Valgrind / instrumentação de destrutores).<br/>"
        "• <b>RNF03 – Modularidade:</b> Arquitetura em camadas com separação rigorosa entre headers (<code>.h</code>) e implementações (<code>.cpp</code>).",
        body_style
    ))

    # 4. Lista de Atividades e Cronograma
    story.append(Paragraph("4. Lista de Atividades (WBS / EAP) e Cronograma de Desenvolvimento", h1_style))
    
    cron_data = [
        [Paragraph("<b>Semana</b>", body_bold), Paragraph("<b>Fase / Marco</b>", body_bold), Paragraph("<b>Atividades e Entregáveis Previstos</b>", body_bold), Paragraph("<b>Meta / Peso</b>", body_bold)],
        [
            Paragraph("Semana 10", body_style),
            Paragraph("Proposta e Modelagem Inicial", body_style),
            Paragraph("Apresentação formal da proposta, diagrama de classes preliminar e arquitetura da FSM.", body_style),
            Paragraph("Aprovação da Proposta", body_style)
        ],
        [
            Paragraph("Semanas 11-12", body_style),
            Paragraph("Entrega 1: Núcleo da FSM", body_style),
            Paragraph("Implementação da classe <code>FSM_Controlador</code>, testes de transições e relatório parcial 1.", body_style),
            Paragraph("<b>Entrega 1 (20%)</b>", body_style)
        ],
        [
            Paragraph("Semana 13", body_style),
            Paragraph("Entrega 2: Estruturas Dinâmicas", body_style),
            Paragraph("Desenvolvimento da classe template <code>FilaEncadeada&lt;T&gt;</code>, classes de exceção e testes unitários.", body_style),
            Paragraph("<b>Entrega 2 (20%)</b>", body_style)
        ],
        [
            Paragraph("Semana 14", body_style),
            Paragraph("Entrega 3: Polimorfismo e Tarifação", body_style),
            Paragraph("Hierarquia <code>Veiculo</code> (Carro, Moto, Caminhão), sobrecarga de <code>operator&lt;&lt;</code> e testes de tarifação.", body_style),
            Paragraph("<b>Entrega 3 (20%)</b>", body_style)
        ],
        [
            Paragraph("Semanas 15-16", body_style),
            Paragraph("Entrega Final: Sistema Integrado", body_style),
            Paragraph("Integração do sistema na classe <code>Estacionamento</code>, bateria completa de testes, código no GitHub e relatório técnico.", body_style),
            Paragraph("<b>Entrega 4 (40%)</b>", body_style)
        ],
        [
            Paragraph("Semana 17", body_style),
            Paragraph("Demonstração Prática", body_style),
            Paragraph("Apresentação ao vivo do sistema compilado e operacional para os professores.", body_style),
            Paragraph("Avaliação Final", body_style)
        ]
    ]

    t_cron = Table(cron_data, colWidths=[65, 120, 247, 100])
    t_cron.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#EDF2F7")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#CBD5E0")),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
        ('LEFTPADDING', (0,0), (-1,-1), 4),
        ('RIGHTPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_cron)
    story.append(Spacer(1, 8))

    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"PDF gerado com sucesso: {filename}")

if __name__ == "__main__":
    build_pdf()
