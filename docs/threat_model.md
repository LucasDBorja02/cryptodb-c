# Threat Model (rascunho)
Este projeto é educacional. A ideia é documentar claramente quais ameaças são cobertas e quais não são.

## Atacante considerado
- A1: Obtém dump do banco (acesso a `BYTEA` cifrado), mas não tem a chave.
- A2: Lê logs/aplicação parcialmente (risco de vazamento de chave).
- A3: Observa padrões (tenta inferir CPF repetido etc.).

## Coberturas
- Dados sensíveis em repouso não aparecem em claro no banco.
- IV aleatório por campo reduz repetição (CBC).

## NÃO coberto (limitações)
- Se a chave for comprometida, confidencialidade cai.
- Algoritmo autoral não é auditado pela comunidade (não-prod).
- Sem autenticação (AEAD): CBC sozinho não garante integridade contra adulteração.
  (Próximo passo: adicionar HMAC ou migrar para modo autenticado).

## Recomendação futura
- Trocar CBC+PKCS7 por AEAD (AES-GCM/ChaCha20-Poly1305) para produção.
