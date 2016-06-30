/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_rtmp.h>
#include <ngx_rtmp_cmd_module.h>

ngx_int_t
ngx_rtmp_parse_tcurl(ngx_rtmp_session_t *s, ngx_rtmp_connect_t *v)
{
    if (!v || !v->tc_url){
        return NGX_ERROR;
    }
    u_char* tc_url = v->tc_url;
    //tc_url = "rtmp://vhost/myapp" or "rtmp://192.168.8.8/myapp?vhost=vhost"
    u_char* start = (u_char*)ngx_strstr(tc_url, "vhost=");
    u_char* end = NULL;
    u_char* pstart = NULL;
    u_char* pend   = NULL;
    ngx_int_t      port = 0;
    if (start) {
        start += ngx_strlen("vhost=");
        end = (u_char *)ngx_strstr(start, "&");
        if (!end) {
            end = start + ngx_strlen(start);
        }
    }
    else {
        start = (u_char *)ngx_strstr(tc_url, "://");
        if (!start){
            return NGX_ERROR;
        }
        start += ngx_strlen("://");

        end = (u_char *)ngx_strchr(start, '/');
        u_char* colon = (u_char *)ngx_strchr(start, ':');

        if (colon){
            end = colon;
        }

        if (!end){
            end = start + ngx_strlen(start) + 1;
        }   
    }
 
    pstart = (u_char *)ngx_strstr(tc_url, "://");
    if (!pstart) {
        return NGX_ERROR;
    }
    pstart += ngx_strlen("://");
    pstart =  (u_char *)ngx_strstr(pstart, ":");
    if(pstart) {
        pstart += 1;   
        pend = (u_char *)ngx_strstr(pstart, "/"); 
        port = ngx_atoi(pstart, pend ? (ngx_uint_t)(pend - pstart) : ngx_strlen(pstart));
    }
    s->lport = port > 0 ?  port : 1935; 

    s->host_in.data = ngx_palloc(s->connection->pool, end - start);
    s->host_in.len = end - start;
    if (s->host_in.data == NULL) {
        return NGX_ERROR;
    }
    ngx_memcpy(s->host_in.data, start, end - start);

    return NGX_OK;
}


