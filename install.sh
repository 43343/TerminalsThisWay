#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

SERVICE_FILE="/etc/systemd/system/ttw.service"
BASH_SCRIPT="/usr/local/bin/ttw"
EXECUTABLE_SOURCE="$SCRIPT_DIR/buildClang/bin/TerminalsThisWay"
EXECUTABLE_DEST="/usr/local/bin/TerminalsThisWay"

echo "Creating $SERVICE_FILE..."
sudo bash -c "cat > $SERVICE_FILE" <<EOL
[Unit]
Description=TTW service
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/bin/TerminalsThisWay
Restart=on-failure
User=your-username
Group=your-groupname

[Install]
WantedBy=multi-user.target
EOL

echo "Creating $BASH_SCRIPT..."
sudo bash -c "cat > $BASH_SCRIPT" <<'EOL'
#! /bin/bash
if [[ "#1" == "--updateConfig" ]]; then
  PID=$(pidof TerminalsThisWay)
  if [[ -n "$PID" ]]; then
    sudo kill -SIGUSR1 "$PID"
    echo "Configuration updated"
  else
    echo "TerminalsThisWay is not running"
  fi
elif [[ "$1" == "--version" ]]; then
  /usr/local/bin/TerminalsThisWay --version
else
  echo "Usage: ttw --updateConfig | --version"
fi 
EOL
echo "Setting execute permissions for $BASH_SCRIPT"
sudo chmod +x $BASH_SCRIPT
echo "Copying TerminalsThisWay to $EXECUTABLE_DEST..."
sudo cp $EXECUTABLE_SOURCE $EXECUTABLE_DEST

echo "Reloading systemd daemon"
sudo systemctl daemon-reload
echo "Installation complete."
