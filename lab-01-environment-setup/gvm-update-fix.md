# GVM Feeds Stuck at "Update in Progress" — Troubleshooting Guide (Kali Linux)

This guide helps you fix common issues when GVM feeds (SCAP, CERT, GVMD_DATA) stay stuck in "Update in progress" — especially after a fresh install of Greenbone (GVM) on Kali Linux.

---

## Full Step-by-Step Process

After installing GVM (`sudo apt install gvm && sudo gvm-setup`), follow this process if feed statuses are stuck or `gvmd --rebuild` fails.

### 1. Try to rebuild NVT cache:

```
sudo runuser -u _gvm -- gvmd --rebuild
```

---

### 2. Common Errors and What They Mean

#### Error: Scanner not reachable

```
Failed to connect to OSP VT update socket.
Failed to rebuild NVT cache.
```

→ `gvmd` cannot find or connect to the scanner. Most likely, it is trying to use an incorrect socket path.

#### Error: Scanner is busy

```
Failed to get scanner_version.
Failed to rebuild NVT cache.
```

→ Another background process (like an automatic feed sync) is using the scanner. Wait 1–3 minutes and try again.

#### Error: Command is killed

```
Killed
```

→ The system ran out of RAM and/or swap space during the rebuild process.

**Solution:**

- Allocate more RAM to your VM (at least 3–4 GB recommended)
- Or add extra swap space:

```bash
# Example: Add 2 GB swap file
sudo fallocate -l 2G /swapfile2
sudo chmod 600 /swapfile2
sudo mkswap /swapfile2
sudo swapon /swapfile2
```

Check current RAM and swap:

```
free -h
```

---

### 3. Check what scanner path `gvmd` is using:

```
sudo runuser -u _gvm -- gvmd --get-scanners
```

Example output:

```
08b69003-5fc2-4037-a479-93b440211c73  OpenVAS  /run/ospd/ospd.sock  0  OpenVAS Default
```

If the path is `/run/ospd/ospd.sock` or port is `0`, this is incorrect.

---

### 4. Check the expected socket path from the scanner configuration:

```
grep unix_socket /etc/gvm/ospd-openvas.conf
```

Expected output:

```
unix_socket = /run/ospd/ospd-openvas.sock
```

This is where the `ospd-openvas` service will create the socket **when it runs**.

Now check if the socket file exists:

```
ls -l /run/ospd/ospd-openvas.sock
```

If the file does **not exist**, it usually means the `ospd-openvas` service is not running yet. Start it with:

```
sudo systemctl start ospd-openvas
```

Then check the socket again. Once it exists — and if `gvmd` is still pointing to the wrong path — that’s the actual problem.

---

### 5. Fix the scanner socket path in `gvmd`:

Take the scanner UUID from step 3 and run:

```
sudo runuser -u _gvm -- gvmd --modify-scanner="UUID" --scanner-host=/run/ospd/ospd-openvas.sock --scanner-port=9390
```

Example:

```
sudo runuser -u _gvm -- gvmd --modify-scanner="08b69003-5fc2-4037-a479-93b440211c73" --scanner-host=/run/ospd/ospd-openvas.sock --scanner-port=9390
```

You should see:

```
Scanner modified.
```

---

### 6. Retry rebuild:

```
sudo runuser -u _gvm -- gvmd --rebuild
```

If it completes with no error — everything is now correctly configured.

---

## Notes

- The socket path used by `ospd-openvas` is defined in:
  `/etc/gvm/ospd-openvas.conf` (look for `unix_socket = ...`)
- `gvmd` stores its scanner configuration in its internal database. On fresh installs, it may point to an outdated path like `/run/ospd/ospd.sock`.
- You must ensure that both components (`ospd-openvas` and `gvmd`) agree on the same socket path.
- Always start your debugging with:
  - `gvmd --get-scanners`
  - `grep unix_socket /etc/gvm/ospd-openvas.conf`
  - And make them match.
