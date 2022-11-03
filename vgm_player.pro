#//////////////////////////////////////////////////
#//                                              //
#// vgm-player                                   //
#// by thorsten kattanek                         //
#//                                              //
#// #file: vgm-player.pro                        //
#//                                              //
#// last changes at 11-02-2022                   //
#// https://github.com/ThKattanek/vgm_player     //
#//                                              //
#//////////////////////////////////////////////////


TEMPLATE = subdirs

  SUBDIRS = src

  emu64.subdir = src

  CONFIG += ordered

# Example for more subdirs
# SUBDIRS = src1\
#            src2
#
# src1.subdir = src1
# src2.subdir = src2
#
# app.depends = src1 src2
