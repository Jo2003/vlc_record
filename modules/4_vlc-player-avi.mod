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
#  - HTTP_PLAY:    play stream from http source                                #
#  - RTSP_PLAY:    play stream from rtsp source                                #
#  - HTTP_REC:     record stream from http source                              #
#  - RTSP_REC:     record stream from rtsp source                              #
#  - HTTP_SIL_REC: silent record from http source                              #
#  - RTSP_SIL_REC: silent record from rtsp source                              #
#  - TRANSLIT:     translit save file name                                     #
# Also there are some placeholders which will be replaced with file names and  #
# options:                                                                     #
# {[%PLAYER%]} - full path to the player program                               #
# {[%URL%]}    - the stream url to play                                        #
# {[%CACHE%]}  - the cache value to use                                        #
# {[%MUX%]}    - format the output stream should be muxed to                   #
# {[%DST%]}    - the file the output stream should be saved to                 #
################################################################################

################################################################################
# special module for use with VideoLAN vlc player which always records AVI     #
################################################################################

;-------------------------------------------------------------------------------
; Do we need to translit the target filename or does the player
; handle cyrillic file names?
; Options: yes, no
;-------------------------------------------------------------------------------
TRANSLIT     = <<no>>

;-------------------------------------------------------------------------------
: force output format no matter what is given as settings
; Options: no, mp4, avi, ts, ...
;-------------------------------------------------------------------------------
FORCE_MUX    = <<avi>>

HTTP_PLAY    = <<"{[%PLAYER%]}" -I dummy {[%URL%]} --no-http-reconnect --http-caching={[%CACHE%]}>>
RTSP_PLAY    = <<"{[%PLAYER%]}" -I dummy {[%URL%]} --rtsp-tcp --rtsp-caching={[%CACHE%]}>>
HTTP_REC     = <<"{[%PLAYER%]}" -I dummy {[%URL%]} --no-http-reconnect --http-caching={[%CACHE%]} --sout="#duplicate{dst=display,dst=std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}}">>
RTSP_REC     = <<"{[%PLAYER%]}" -I dummy {[%URL%]} --rtsp-tcp --rtsp-caching={[%CACHE%]} --sout="#duplicate{dst=display,dst=std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}}">>
HTTP_SIL_REC = <<"{[%PLAYER%]}" -I dummy {[%URL%]} --no-http-reconnect --http-caching={[%CACHE%]} --sout="#std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}">>
RTSP_SIL_REC = <<"{[%PLAYER%]}" -I dummy {[%URL%]} --rtsp-tcp --rtsp-caching={[%CACHE%]} --sout="#std{access=file,mux=ffmpeg{mux={[%MUX%]}},dst='{[%DST%]}.{[%MUX%]}'}">>
