usr=$3
url=$1
dir=$2
dirTar=$2"import.tar"
dirList=$2"import.list"
urlDirTar=$3"@"$1":"$2"import.tar"
urlDirList=$3"@"$1":"$2"import.list"
# echo "url:"$url
# echo "dirTar:"$dirTar
# echo "dirList:"$dirList
# echo "urlDirTar:"$urlDirTar
# echo "urlDirList:"$urlDirList
# sudo docker commit
sudo docker save $(docker images -q) -o $dirTar
sudo docker images | sed '1d' | awk '{print $1 " " $2 " " $3}' > $dirList
scp $dirTar $urlDirTar
scp $dirList $urlDirList
# sudo docker load -i $dirTar
# while read REPOSITORY TAG IMAGE_ID
# do
#         echo "== Tagging $REPOSITORY $TAG $IMAGE_ID =="
#         sudo docker tag "$IMAGE_ID" "$REPOSITORY:$TAG"
# done < $dirList
exit 0

# AppId="$1"
# AppDir="../../AppsMinion/""$AppId/"
# AppContainer=`docker ps -a | grep "$AppId" | awk '{print $1 }'`
# AppImage=`docker images -a | grep "$AppId" | awk '{ print $3}'`

# sudo docker stop $AppContainer
# sudo docker rm $AppContainer
# sudo docker rmi -f $AppImage
# rm -rf "$AppDir"
# exit 0
