# OpenClaw Agent Configuration: Sailfish OS (SFOS) Profile

## Environment & Execution Engine
- **Platform:** Sailfish OS (Linux-based mobile environment, Lipstick compositor, Wayland)
- **Local Provider:** Ollama / Local network endpoint via WLAN (e.g., serving `glm-4-flash` or `llama3`)
- **Execution Mode:** Native CLI / background daemon with D-Bus capability access

## Permissions & Native Capabilities
- **Contacts:** Enabled (Read/Write via `pkcon` / `contacts-sqlite` or native `telepathy-mission-control`)
- **SMS / Messaging:** Enabled (Direct interface via `ofono` / `connman` D-Bus bindings)
- **Filesystem Scope:** `/home/nemo/.local/share/openclaw/` and `/home/nemo/Documents/`

## Available Tools & Skills
1. **SMS Skill:** Send/receive text messages via local `ofono` D-Bus commands.
2. **Contacts Skill:** Query local address book for quick-dial or automated messaging intents.
3. **WLAN Bridge:** Route compute-heavy agent loops to a local desktop/laptop running heavier models (e.g., Qwen/GLM) over a secure local WLAN socket, keeping light execution native on the phone.

## Safety & Guardrails
- **Confirmation Required:** Any destructive file operation or outbound SMS/financial API call requires a user-prompt notification via the Sailfish Silica UI notification banner.
- **Token / Cost Guardrail:** Restrict autonomous loops to local weights when off-grid; use cloud fallbacks only with explicit user confirmation.
