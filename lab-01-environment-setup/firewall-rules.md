## Firewall Rules Configuration (GUI)

1. Open the firewall settings in the EdgeRouterX web interface:  
   **Firewall/NAT > Firewall Policies**

2. Create a new ruleset named **WAN_OUT** with:

   - **Default Action**: `Drop`

3. Assign the **WAN_OUT** ruleset to the **WAN interface (eth4)**

4. Add the following allow rules:

- **HTTP** (TCP port 80) → `Action: Accept`
- **HTTPS** (TCP port 443) → `Action: Accept`
- **DNS** (TCP/UDP port 53) → `Action: Accept`
- **Rsync** (TCP port 873) → `Action: Accept` // For GVM feed updates
- **NTP** (UDP port 123) → `Action: Accept` // For Raspberry Pi time sync
