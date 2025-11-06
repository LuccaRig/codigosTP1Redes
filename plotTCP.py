import matplotlib.pyplot as plt
import numpy as np

def ler_arquivo(nome_arquivo):
    """Lê o arquivo de dados e retorna arrays de tempo e CongWin"""
    tempos = []
    congwins = []
    
    try:
        with open(nome_arquivo, 'r') as arquivo:
            for linha in arquivo:
                # Remove espaços em branco e divide a linha
                dados = linha.strip().split()
                if len(dados) >= 2:
                    tempo = float(dados[0])
                    congwin = float(dados[1])
                    tempos.append(tempo)
                    congwins.append(congwin)
        
        return np.array(tempos), np.array(congwins)
    
    except FileNotFoundError:
        print(f"Erro: Arquivo {nome_arquivo} não encontrado!")
        return None, None
    except Exception as e:
        print(f"Erro ao ler o arquivo {nome_arquivo}: {e}")
        return None, None

def plotar_grafico():
    """Lê os dados e plota o gráfico comparativo"""
    # Lê os dados dos dois arquivos
    tempo_newreno, congwin_newreno = ler_arquivo('ns3::TcpNewRenolab2-part1-flow0-cwnd.data')
    tempo_cubic, congwin_cubic = ler_arquivo('ns3::TcpCubiclab2-part1-flow0-cwnd.data')
    # Verifica se os dados foram carregados corretamente
    if tempo_newreno is None or tempo_cubic is None:
        print("Não foi possível carregar os dados. Verifique os arquivos.")
        return
    
    # Converte CongWin de bytes para MB (opcional - ajuste conforme necessário)
    congwin_newreno_mb = congwin_newreno / (1024 * 1024)
    congwin_cubic_mb = congwin_cubic / (1024 * 1024)
    
    # Cria o gráfico
    plt.figure(figsize=(12, 6))
    
    # Plota as duas curvas
    plt.plot(tempo_newreno, congwin_newreno_mb, 'b-', label='NewReno', linewidth=2)
    plt.plot(tempo_cubic, congwin_cubic_mb, 'r-', label='Cubic', linewidth=2)
    
    # Configurações do gráfico
    plt.xlabel('Tempo (s)', fontsize=12)
    plt.ylabel('CongWin (bytes)', fontsize=12)
    plt.title('Comparação do Congestion Window: NewReno vs Cubic', fontsize=14)
    plt.legend(fontsize=12)
    plt.grid(True, alpha=0.3)
    
    # Ajusta os limites do eixo y se necessário (para visualização melhor)
    # plt.ylim(bottom=0)
    
    # Formatação dos eixos
    plt.tight_layout()
    
    # Salva o gráfico (opcional)
    plt.savefig('comparacao_congwin.png', dpi=300, bbox_inches='tight')
    
    # Mostra o gráfico
    plt.show()

def analisar_dados():
    """Função adicional para analisar estatísticas dos dados"""
    tempo_newreno, congwin_newreno = ler_arquivo('NewReno.data')
    tempo_cubic, congwin_cubic = ler_arquivo('Cubic.data')
    
    if tempo_newreno is not None and tempo_cubic is not None:
        print("\n=== Estatísticas dos Dados ===")
        print(f"NewReno - Total de pontos: {len(tempo_newreno)}")
        print(f"NewReno - Tempo total: {tempo_newreno[-1]:.2f}s")
        print(f"NewReno - CongWin máximo: {congwin_newreno.max():.0f} bytes")
        print(f"NewReno - CongWin médio: {congwin_newreno.mean():.0f} bytes")
        
        print(f"\nCubic - Total de pontos: {len(tempo_cubic)}")
        print(f"Cubic - Tempo total: {tempo_cubic[-1]:.2f}s")
        print(f"Cubic - CongWin máximo: {congwin_cubic.max():.0f} bytes")
        print(f"Cubic - CongWin médio: {congwin_cubic.mean():.0f} bytes")

# Executa o programa
if __name__ == "__main__":
    print("Lendo arquivos NewReno.data e Cubic.data...")
    
    # Plota o gráfico
    plotar_grafico()
    
    # Mostra estatísticas (opcional)
    analisar_dados()