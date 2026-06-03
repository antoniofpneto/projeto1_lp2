import random

def gerar_arquivo_texto(nome_arquivo, tamanho_alvo_mb):
    # Semente fixa para reprodutibilidade (simulando a estratégia de semente do random.org)
    random.seed(42)
    
    # Simula um lote inicial baixado do random.org/strings (strings aleatórias de comprimentos variados)
    caracteres = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    lote_random_org = [
        "".join(random.choices(caracteres, k=random.randint(3, 12))) 
        for _ in range(5000)
    ]
    
    tamanho_alvo_bytes = tamanho_alvo_mb * 1024 * 1024
    bytes_escritos = 0
    
    print(f"Gerando '{nome_arquivo}' com ~{tamanho_alvo_mb} MB com base no padrão random.org...")
    
    with open(nome_arquivo, "w", encoding="utf-8") as f:
        while bytes_escritos < tamanho_alvo_bytes:
            # Replica e concatena o lote separando por espaços conforme o enunciado
            linha = " ".join(random.choices(lote_random_org, k=100)) + " "
            f.write(linha)
            bytes_escritos += len(linha.encode('utf-8'))
            
    print(f"Arquivo gerado com sucesso! Tamanho final: {bytes_escritos / (1024*1024):.2f} MB")

if __name__ == "__main__":
    gerar_arquivo_texto("texto_grande.txt", 55)