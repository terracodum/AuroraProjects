#!/bin/bash
 
# Переменные с информацией о проекте
project_namespace=$CI_PROJECT_NAMESPACE
project_name=$CI_PROJECT_NAME
branch_name=$CI_COMMIT_BRANCH
store_path="/home/gitlab-runner/projects-archives-storage"
 
# Создание директории под проект
if [ ! -d "$project_namespace" ]; then
    mkdir -p ${store_path}/${project_namespace}/${project_name}/${branch_name}/icons/
fi
 
# Поиск иконки с разрешением не менее 512x512
cd ./icons/
found_icons_dir=""
found_icons=false
 
for dir in [0-9]*x[0-9]*/; do
    dir=${dir%/}
    width=$(echo "$dir" | cut -d'x' -f1)
    height=$(echo "$dir" | cut -d'x' -f2)
    if [ "$width" -ge 512 ] && [ "$height" -ge 512 ]; then
        found_icons=true
        found_icons_dir=$dir
        break
    fi
done
 
if [[ "$found_icons" == "false" ]]; then
    echo "Иконок с разрешением 512x512 или выше не найдено"
    exit 1
else
    mkdir -p ${store_path}/${project_namespace}/${project_name}/${branch_name}/icons/${found_icons_dir}/
    cp ./${found_icons_dir}/ru.auroraos.${project_name}.png ${store_path}/${project_namespace}/${project_name}/${branch_name}/icons/${found_icons_dir}/
fi
 
# Возвращение в корневую директорию проекта
cd ../
    
 
# Выкачивание git сабмодулей
if [ -f ./.gitmodules ]; then
    submodules_list=$(git config --file .gitmodules --get-regexp path | awk '{print $2}')
    for submodule_path in $submodules_list ;do
        submodule_url=$(git config --file .gitmodules --get "submodule.$submodule_path.url")
        git clone "$submodule_url"
    done
fi
 
rm -rf gitlab-ci/ .git/ .gitignore .gitlab-ci.yml .gitmodules artifacts/
 
# Создаем или инициализируем Meta-файл в формате json
list_example_json_path="${store_path}/список_пример.json"
if [ ! -f "$list_example_json_path" ]; then
    echo "[]" > "$list_example_json_path"
fi
 
# Переменные для meta-файла
ftp_server_address="ftp://10.189.64.241"
name=$project_name
group=$(echo "$project_namespace" | cut -d'/' -f2)
branch=$branch_name
tags=("av-launcher" "antivirus" "антивирус" "Antivirus API")
icon="${store_path}/${project_namespace}/${project_name}/${branch_name}/icons/${found_icons_dir}/ru.auroraos.${project_name}.png"
description=$(grep -A1 "Описание" README.ru.md | grep -v "Описание")
repo_url=$CI_PROJECT_URL
supported_versions=("4.0.1" "4.0.2" "4.1.0" "5.0.0" "5.0.1" "5.1.0" "5.1.1" "5.1.2" "5.1.3" "5.1.4" "5.1.5" "5.1.6" "5.2.0")
 
supported_versions_to_string=$(printf '%s-' "${supported_versions[@]}")
supported_versions_to_string=${supported_versions_to_string%-} # Срез крайнего тире
 
# Новые supported_versions в формате JSON
new_supported_versions_json=$(printf '%s\n' "${supported_versions[@]}" | jq -R . | jq -s .)
 
archive_path=${store_path}/${project_namespace}/${project_name}/${branch_name}/${project_name}${supported_versions_to_string}.7z
if [ -f $archive_path ]; then
    rm $archive_path
fi
# Создание архива
7z a $archive_path .
  
file="${ftp_server_address}/${project_namespace}/${project_name}/${branch_name}/${project_name}${supported_versions_to_string}.7z"
archive_size=$(du -h $archive_path | cut -f1)
 
new_record=$(jq -n --arg name "$name" \
    --arg group "$group" \
    --arg branch "$branch" \
    --arg file "$file" \
    --argjson tags "$(printf '%s\n' "${tags[@]}" | jq -R . | jq -s .)" \
    --arg icon "$icon" \
    --arg description "$description" \
    --arg repo_url "$repo_url" \
    --argjson supported_versions "$(printf '%s\n' "${supported_versions[@]}" | jq -R . | jq -s .)" \
    --arg archive_size "$archive_size" \
    '{ name: $name, group: $group, branch: $branch, file: $file, tags: $tags, icon: $icon, description: $description, repo_url: $repo_url, supported_versions: $supported_versions, archive_size: $archive_size }')
     
 
# Ищем существующую запись с точным совпадением имени, ветки и версий
existing_record=$(jq --arg name "$name" --arg branch "$branch" --argjson new_versions "$new_supported_versions_json" \
    '.[] | select(.name == $name and .branch == $branch and (.supported_versions | sort) == ($new_versions | sort))' \
    "$list_example_json_path")
 
if [ -n "$existing_record" ]; then 
    # Имя, ветка и версии идентичны - обновляем существующую запись
    echo "Обновление существующей записи для приложения: $name ветка $branch $supported_versions_to_string (имя, ветка и версии идентичны)"
    jq --arg name "$name" --arg branch "$branch" --argjson new_versions "$new_supported_versions_json" --argjson new_record "$new_record" \
        'map(if .name == $name and .branch == $branch and (.supported_versions | sort) == ($new_versions | sort) then $new_record else . end)' \
        "$list_example_json_path" > tmp.json && mv tmp.json "$list_example_json_path"
else
    # Запись не найдена - создаем новую
    echo "Добавление новой записи для приложения: $name ветка $branch $supported_versions_to_string"
    jq ". += [$new_record]" "$list_example_json_path" > tmp.json && mv tmp.json "$list_example_json_path"
fi