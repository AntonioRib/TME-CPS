usr=$3
url=$1
dir=$2
dirTar=$2"import.tar"
dirList=$2"import.list"
urlDirTar=$3"@"$1":"$2"import.tar"
urlDirList=$3"@"$1":"$2"import.list"
sudo docker save $(docker images -q) -o $dirTar
sudo docker images | sed '1d' | awk '{print $1 " " $2 " " $3}' > $dirList
sudo scp $dirTar $urlDirTar
sudo scp $dirList $urlDirList
exit 0

