################################################################################
# Player module for vlc-record.                                                #
# Goal of this modules is to support multiple player in vlc-record without     #
# the need to change the binary code of this program.                          #
#            !!!!! Please store the files in UTF-8 format !!!!!                #
################################################################################
# In this file you have to put the command line to run the player you like.    #
# In case the player doesn't support all features of vlc-record, make an empty #
# define for this feature.                                                     #
# Defines to use:                                                              #
#  - LIVE_PLAY:    play stream from http source                                #
#  - ARCH_PLAY:    play stream from rtsp source                                #
#  - LIVE_REC:     record stream from http source                              #
#  - ARCH_REC:     record stream from rtsp source                              #
#  - LIVE_SIL_REC: silent record from http source                              #
#  - ARCH_SIL_REC: silent record from rtsp source                              #
#  - TRANSLIT:     translit save file name                                     #
#  - FORCE_MUX:    force mux to this format                                    #
# Also there are some placeholders which will be replaced with file names and  #
# options:                                                                     #
# {[%PLAYER%]} - full path to the player program                               #
# {[%URL%]}    - the stream url to play                                        #
# {[%CACHE%]}  - the cache value to use                                        #
# {[%MUX%]}    - format the output stream should be muxed to                   #
# {[%DST%]}    - the file the output stream should be saved to                 #
################################################################################

###########################################
# module for use with MPlayer             #
###########################################

;-------------------------------------------------------------------------------
; Do we need to translit the target filename or does the player
; handle cyrillic file names?
; Options: yes, no
;-------------------------------------------------------------------------------
TRANSLIT     = <<yes>>

;-------------------------------------------------------------------------------
: force output format no matter what is given as settings
; Options: no, mp4, avi, ts, ...
;-------------------------------------------------------------------------------
FORCE_MUX    = <<no>>

LIVE_PLAY    = <<"{[%PLAYER%]}" {[%URL%]} -cache {[%CACHE%]}>>
ARCH_PLAY    = <<"{[%PLAYER%]}" {[%URL%]} -cache {[%CACHE%]}>>
LIVE_SIL_REC = <<"{[%PLAYER%]}" {[%URL%]} -cache {[%CACHE%]} -dumpstream -dumpfile "{[%DST%]}.{[%MUX%]}">>
ARCH_SIL_REC = <<"{[%PLAYER%]}" {[%URL%]} -cache {[%CACHE%]} -dumpstream -dumpfile "{[%DST%]}.{[%MUX%]}">>

;-------------------------------------------------------------------------------
; Until now I don't know if there is an option to play and dump the stream
; at same time. Therefore I use the same command line for normal record
; as for silent record ...
;-------------------------------------------------------------------------------
LIVE_REC     = <<"{[%PLAYER%]}" {[%URL%]} -cache {[%CACHE%]} -dumpstream -dumpfile "{[%DST%]}.{[%MUX%]}">>
ARCH_REC     = <<"{[%PLAYER%]}" {[%URL%]} -cache {[%CACHE%]} -dumpstream -dumpfile "{[%DST%]}.{[%MUX%]}">>
