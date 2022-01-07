# TwitchInteractionUE
Simple plugin to interact with twitch IRC and pubsub from Unreal Engine 5

# About
This plugin adds some C++ code and components that can be used to send and receive twitch chat messages, chat commands and received twitch bits/redeem/subscribe notifications.
The plugin was built in UE5 Early Access but is also compatible with UE 4.27, The example blueprints are just invalid in UE4.

# Getting Started

Add the plugin to your project by creating a plugins folder in your project root and adding the contents of this repo into a TwitchInteractionUE folder in the plugins folder.
(The repo is structured so that it can easily be pulled into your plugins folder as a git submodule)

![image](https://user-images.githubusercontent.com/24256332/148518008-b3d0d1df-7cb0-4e9d-8e14-a45467428e8e.png)

Then go to your project plugins menu and activate it.

![image](https://user-images.githubusercontent.com/24256332/148518243-a52b4352-3cf0-4c4f-852e-197f3e8101ae.png)

Unreal will ask you to restart after which the plugin will be active.

In the plugin content folder there are example blueprints showing the basic setup and usage of the components.

![image](https://user-images.githubusercontent.com/24256332/148520339-d5dfccd0-abad-4700-9614-a8bcae956ab4.png)

When creating a blueprint using this plugin, there are 3 components tha can be used.

![image](https://user-images.githubusercontent.com/24256332/148520579-b7974438-0abe-4db7-9483-74cf90d19470.png)

## Twitch setup

First of all you need to register your app on twitch. For this you can go to dev.twitch.tv/console. Under applications click "Register your Application".
Then fill in the details as you like but the important part here is that the redirect URL is a localhost address that matches the plugin URL (Will be covered later). It defaults to http://localhost:8080/Auth but can be changed as long as it follows the pattern of http://localhost:{port}/{something}. This is where twitch will redirect the browser to for token delivery everytime the app needs to authenticate.
Also make a note the Client ID given to you by twitch after registering you app. You will need it for the authentication token in Unreal Engine

![image](https://user-images.githubusercontent.com/24256332/148522651-73f18288-4c2d-4726-ad8a-27939ce01cf1.png)

###### Twitch Authentication Component
The Authentication component handles the protocol of getting an authentication token from twitch which is used to login to either IRC (Twitch chat) or pub sub (events).

Twitch requires a user to confirm access to a new application on twitch's website and this confirmation then redirects to a url to deliver the authentication token. To facilitate that process, the twitch authentication component will run a local webserver that twitch can redirect to.

This component has values that can be set directly or through blueprints but they are the following:

![image](https://user-images.githubusercontent.com/24256332/148523153-342a9533-d6b8-480d-a429-d764d775b893.png)

The redirect and listen port needs to match what you setup in twitch (see above twitch setup) and needs to be localhost. The Client Id needs to also match what was given by twitch from the above setup. Username should be your username that you will use to log the app in on twitch. The Userid and token can be left blank because they will be retreived on login.
The default scopes should work for most scenarios but if you want to change them you can lookup scopes and what they do on https://dev.twitch.tv/docs/authentication#scopes

You can connect an Pubsub component or Chat component or both in the construction script/begin play of your blueprint so the Authentication component can populate important variables for their use upon login.

![image](https://user-images.githubusercontent.com/24256332/148524126-96d6834d-a78d-4b58-aa86-c76d505986cd.png)

There are these events that you can bind to and handle accordingly.

![image](https://user-images.githubusercontent.com/24256332/148524374-5e60e48f-0383-4e35-96b0-8d1c2cae108b.png)

See the example blueprints for a full login flow as well as saving the token to avoid having to login everytime you start when you still have a valid token.


