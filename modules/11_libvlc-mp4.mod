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

################################################################################
# module for use with VideoLANs libvlc integrated into vlc-record              #
################################################################################

;-------------------------------------------------------------------------------
; Do we need to translit the target filename or does the player
; handle cyrillic file names?
; Options: yes, no
;-------------------------------------------------------------------------------
TRANSLIT     = <<no>>

;-------------------------------------------------------------------------------
; force output format no matter what is given as settings
; Options: no, mp4, avi, ts, ...
;-------------------------------------------------------------------------------
FORCE_MUX    = <<mp4>>

;-------------------------------------------------------------------------------
; Note the special cut separator ";;"!
; This is needed only if we use libvlc because we have to splitt the
; command line into tokens. Don't use a space as cut char cause it can be
; a part of the destination path!
;-------------------------------------------------------------------------------
; In a short: Separate command line arguments with ";;" if the mod file is
; for use with libvlc!
;-------------------------------------------------------------------------------
LIVE_PLAY    = <<{[%URL%]};;:no-http-reconnect;;:http-caching={[%CACHE%]}>>
ARCH_PLAY    = <<{[%URL%]};;:no-http-reconnect;;:run-time=36000;;:http-caching={[%CACHE%]}>>
LIVE_REC     = <<{[%URL%]};;:no-http-reconnect;;:http-caching={[%CACHE%]};;:sout=#duplicate{dst=display,dst=std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}}>>
ARCH_REC     = <<{[%URL%]};;:no-http-reconnect;;:run-time=36000;;:http-caching={[%CACHE%]};;:sout=#duplicate{dst=display,dst=std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}}>>
LIVE_SIL_REC = <<{[%URL%]};;:no-http-reconnect;;:http-caching={[%CACHE%]};;:sout=#std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}>>
ARCH_SIL_REC = <<{[%URL%]};;:no-http-reconnect;;:run-time=36000;;:http-caching={[%CACHE%]};;:sout=#std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}>>
