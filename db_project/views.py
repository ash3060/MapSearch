# -*- coding: utf-8 -*-
from django.shortcuts import render_to_response
from django.http import HttpResponse
from django.conf import settings
from django.template import RequestContext

sort_list = []


def search_list(latmin, latmax, lngmin, lngmax):
    inverse_list = {}
    global sort_list
    sort_list = []
    for entry in settings.RECORD_LIST:
        # print entry.lat, entry.lng
        if (entry.lat > latmin) and (entry.lat < latmax):
            if (entry.lng > lngmin) and (entry.lng < lngmax):
                # print entry.lat, entry.lng, "in area"
                for word in entry.tags:
                    # print word
                    if word in inverse_list:
                        inverse_list[word] = inverse_list[word] + 1
                    else:
                        inverse_list[word] = 1
    sort_list = sorted(inverse_list.items(), key=lambda e: e[1], reverse=True)


def search(request):
    if "bound1" in request.GET and request.GET["bound1"]:
        bound1 = request.GET["bound1"]
    else:
        return HttpResponse("Please enter bounds and k")

    if "bound2" in request.GET and request.GET["bound2"]:
        bound2 = request.GET["bound2"]
    else:
        return HttpResponse("Please enter bounds and k")

    if "k" in request.GET and request.GET["k"]:
        k = int(request.GET["k"])
    else:
        return HttpResponse("Please enter bounds and k")

    lat1 = float(bound1.split(',')[0])
    lng1 = float(bound1.split(',')[1])
    lat2 = float(bound2.split(',')[0])
    lng2 = float(bound2.split(',')[1])
    latmin = min(lat1, lat2)
    latmax = max(lat1, lat2)
    lngmin = min(lng1, lng2)
    lngmax = max(lng1, lng2)
    print latmin, latmax, lngmin, lngmax
    search_list(latmin, latmax, lngmin, lngmax)
    count = 0
    lis = []
    for item in sort_list:
        lis = lis + [item]
        count = count + 1
        if count == k:
            break

    return render_to_response("result.html", {'lis': lis})


def index(request):
    context = {}
    return render_to_response('index.html', RequestContext(request, context))
