# usr=$3
# url=$1
dir=$1
dirTar=$1".tar"
dirList=$1".list"
# urlDirTar=$3"@"$1":"$2".tar"
# urlDirList=$3"@"$1":"$2".list"
# echo "url:"$url
# echo "dirTar:"$dirTar
# echo "dirList:"$dirList
# echo "urlDirTar:"$urlDirTar
# echo "urlDirList:"$urlDirList
# sudo docker commit
# sudo docker save $(docker images -q) -o $dirTar
# sudo docker images | sed '1d' | awk '{print $1 " " $2 " " $3}' > $dirList
# sudo scp $dirTar $urlDirTar
# sudo scp $dirList $urlDirList
sudo docker load -i $dirTar
while read REPOSITORY TAG IMAGE_ID
do
        echo "== Tagging $REPOSITORY $TAG $IMAGE_ID =="
        sudo docker tag "$IMAGE_ID" "$REPOSITORY:$TAG"
done < $dirList
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
