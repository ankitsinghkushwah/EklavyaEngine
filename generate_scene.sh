#!/bin/bash

# Check if the Scene Name is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <SceneName>"
  exit 1
fi

# Define source and destination directories
TEMPLATE_DIR="SceneTemplate"
SCENE_NAME=$1
DEST_DIR="Demos/$SCENE_NAME"

# Create the destination directory
mkdir -p "$DEST_DIR"

# Copy template files and rename them with SceneNameDemo
cp "$TEMPLATE_DIR/SceneTemplate.h" "$DEST_DIR/${SCENE_NAME}Demo.h"
cp "$TEMPLATE_DIR/SceneTemplate.cpp" "$DEST_DIR/${SCENE_NAME}Demo.cpp"

# Update header guard in the .h file
HEADER_GUARD="Inc${SCENE_NAME}Demo_h"
UPPER_HEADER_GUARD=$(echo "$HEADER_GUARD" | tr '[:lower:]' '[:upper:]')
sed -i "" "s/SCENETEMPLATE_H/${UPPER_HEADER_GUARD}/g" "$DEST_DIR/${SCENE_NAME}Demo.h"

# Rename all occurrences of SceneTemplate to SceneNameDemo in .h and .cpp files
sed -i "" "s/SceneTemplate/${SCENE_NAME}Demo/g" "$DEST_DIR/${SCENE_NAME}Demo.h"
sed -i "" "s/SceneTemplate/${SCENE_NAME}Demo/g" "$DEST_DIR/${SCENE_NAME}Demo.cpp"

git add "$DEST_DIR/${SCENE_NAME}Demo.h"
git add "$DEST_DIR/${SCENE_NAME}Demo.cpp"

echo "Scene '$SCENE_NAME' created successfully in '$DEST_DIR'"