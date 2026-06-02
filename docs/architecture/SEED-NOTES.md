# SEED-NOTES — PaperClock

First architecture artifact (batch 2 of the producer backfill). See
`../../BATCH-2-FINDINGS.md` and `../../REVIEW-2.md` for the shared rationale; this
file records what is specific to this repo.

- **Producer id:** `paper-clock`  ·  **introduced:** `2024-03-29` (repo's first commit, full clone).
- **What this repo owns:** the FIRMWARE «SoftwareProduct» only. The physical device is registered in IoT Support and will be generated there as a `device:` instance — not modeled here.
- **Universal MDM edges (no boundBy):** firmware → `svc:iotsupport-api` (device API),
  → `cap:pub-sub-broker` (MQTT), → `cap:iam` (Keycloak M2M). Addresses are written
  into device NVS by IoT Support from its own config, so the firmware carries no
  `boundBy` recipe; IoT Support's planned device generator will emit the realized
  per-device `Serving` edges. Modeled as element kind **SystemSoftware** (bare-metal
  firmware) so a later `device: —Assignment→ ss:` resolves cleanly.
- **Device-specific:** none — weather/printer/climate data all arrive over MQTT topics.
- **Home Assistant (over MQTT):** → `svc:home-assistant-mqtt`. Consumes the weather/printer/climate data a custom Home Assistant **AppDaemon** script publishes to MQTT (the script is an internal HA automation, not a modeled element); no boundBy.
- **Validation:** `./scripts/arch-validate.py docs/architecture/architecture.yaml` → OK.
