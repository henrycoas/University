@rocsalvador

# global top URI: spotify:playlist:37i9dQZEVXbMDoHDwVN2tF

try:
    import spotipy
    import pandas as pd
except:
    print("Install spotipy and pandas libraries:\n$ pip3 install spotipy pandas")
    exit()

from spotipy.oauth2 import SpotifyClientCredentials

client_credentials_manager = SpotifyClientCredentials('credentials', 'credentials')
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)

playlist_id = input("Insert playlist URI: ")
playlist_id = playlist_id[17:len(playlist_id)]

content = sp.playlist_items(playlist_id)
playlist_data = []
for i in content['items']:
    track_id = i['track']['id']
    features = sp.audio_features(track_id)
    danceability = features[0]['danceability']
    valence = features[0]['valence']
    energy = features[0]['energy']
    name = i['track']['name']
    popularity = sp.track(track_id)['popularity']
    playlist_data.append([name, popularity, danceability, valence, energy])

df = pd.DataFrame(playlist_data, columns=['name', 'popularity', 'danceability', 'valence', 'energy'])
df.to_csv('data.csv', sep=';', index=False)
