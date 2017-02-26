CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(spine)

SET( SOURCE_DIR ${THIRDPARTY_DIR}/spine-c/spine-c )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/spine/Animation.c
${SOURCE_DIR}/src/spine/AnimationState.c
${SOURCE_DIR}/src/spine/AnimationStateData.c
${SOURCE_DIR}/src/spine/Atlas.c
${SOURCE_DIR}/src/spine/AtlasAttachmentLoader.c
${SOURCE_DIR}/src/spine/Attachment.c
${SOURCE_DIR}/src/spine/AttachmentLoader.c
${SOURCE_DIR}/src/spine/Bone.c
${SOURCE_DIR}/src/spine/BoneData.c
${SOURCE_DIR}/src/spine/BoundingBoxAttachment.c
${SOURCE_DIR}/src/spine/Event.c
${SOURCE_DIR}/src/spine/EventData.c
${SOURCE_DIR}/src/spine/extension.c
${SOURCE_DIR}/src/spine/IkConstraint.c
${SOURCE_DIR}/src/spine/IkConstraintData.c
${SOURCE_DIR}/src/spine/Json.c
${SOURCE_DIR}/src/spine/Json.h
${SOURCE_DIR}/src/spine/kvec.h
${SOURCE_DIR}/src/spine/MeshAttachment.c
${SOURCE_DIR}/src/spine/PathAttachment.c
${SOURCE_DIR}/src/spine/PathConstraint.c
${SOURCE_DIR}/src/spine/PathConstraintData.c
${SOURCE_DIR}/src/spine/RegionAttachment.c
${SOURCE_DIR}/src/spine/Skeleton.c
${SOURCE_DIR}/src/spine/SkeletonBinary.c
${SOURCE_DIR}/src/spine/SkeletonBounds.c
${SOURCE_DIR}/src/spine/SkeletonData.c
${SOURCE_DIR}/src/spine/SkeletonJson.c
${SOURCE_DIR}/src/spine/Skin.c
${SOURCE_DIR}/src/spine/Slot.c
${SOURCE_DIR}/src/spine/SlotData.c
${SOURCE_DIR}/src/spine/TransformConstraint.c
${SOURCE_DIR}/src/spine/TransformConstraintData.c
${SOURCE_DIR}/src/spine/VertexAttachment.c
)             
              
include_directories(${SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../stdex/include)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )

