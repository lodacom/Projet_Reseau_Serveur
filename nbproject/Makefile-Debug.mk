#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Serveur.o \
	${OBJECTDIR}/sockdist.o \
	${OBJECTDIR}/sock.o \
	${OBJECTDIR}/sauvegarde.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread `pkg-config --cflags glib-2.0` 
CXXFLAGS=-pthread `pkg-config --cflags glib-2.0` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs glib-2.0`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/projet_reseau_serveur

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/projet_reseau_serveur: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/projet_reseau_serveur ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Serveur.o: Serveur.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g `pkg-config --cflags glib-2.0`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/Serveur.o Serveur.cpp

${OBJECTDIR}/sockdist.o: sockdist.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g `pkg-config --cflags glib-2.0`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/sockdist.o sockdist.cpp

${OBJECTDIR}/sock.o: sock.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g `pkg-config --cflags glib-2.0`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/sock.o sock.cpp

${OBJECTDIR}/sauvegarde.o: sauvegarde.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags glib-2.0`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/sauvegarde.o sauvegarde.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/projet_reseau_serveur

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
