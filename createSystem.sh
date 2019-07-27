mkdir -p src/Servers/Minion/AppsImport
mkdir -p src/Servers/Minion/AppsMinion
mkdir -p src/Servers/MinionSGX/AppsImport
mkdir -p src/Servers/MinionSGX/AppsMinion

mkdir -p src/Servers/Monitor/AppsMonitor
mkdir -p src/Servers/MonitorSGX/AppsMonitor

mkdir -p Logs
mkdir -p Logs/Commited
mkdir -p Logs/Uncommited

rm -r src/Servers/Minion/AppsImport/*
rm -r src/Servers/Minion/AppsMinion/*
rm -r src/Servers/MinionSGX/AppsImport/*
rm -r src/Servers/MinionSGX/AppsMinion/*

rm -r src/Servers/Monitor/AppsMonitor/*
rm -r src/Servers/MonitorSGX/AppsMonitor/*

rm -r Logs/Commited/*
rm -r Logs/Uncommited/*

chmod +x DeleteApp.sh
chmod +x Export.sh
chmod +x Import.sh
chmod +x PurgeMinion.sh

chmod +x src/Servers/Minion/DeleteApp.sh
chmod +x src/Servers/Minion/Export.sh
chmod +x src/Servers/Minion/Import.sh
chmod +x src/Servers/Minion/PurgeMinion.sh

chmod +x src/Servers/MinionSGX/DeleteApp.sh
chmod +x src/Servers/MinionSGX/Export.sh
chmod +x src/Servers/MinionSGX/Import.sh
chmod +x src/Servers/MinionSGX/PurgeMinion.sh