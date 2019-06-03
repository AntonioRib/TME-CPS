dir=$1
dirTar=$1".tar"
dirList=$1".list"
sudo docker load -i $dirTar
while read REPOSITORY TAG IMAGE_ID
do
        echo "== Tagging $REPOSITORY $TAG $IMAGE_ID =="
        sudo docker tag "$IMAGE_ID" "$REPOSITORY:$TAG"
done < $dirList
exit 0
