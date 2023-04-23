echo "Starting to generate config file"
cat ha_header.yaml.template > generatedHaEntity.yaml

for id in `cat deviceIds.txt`
do
  ID_WITH_NO_WHITESPACE=`echo $id | sed -e 's/[[:space:]]*$//'`
  echo "Adding config for id ${ID_WITH_NO_WHITESPACE}"
  cat ha_entity.yaml.template | sed "s/#DEVICE_ID#/${ID_WITH_NO_WHITESPACE}/g" >> generatedHaEntity.yaml
  echo "Done adding config"

done
