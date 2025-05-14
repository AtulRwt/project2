const axios = require('axios');
const captainModel = require('../models/captain.model');

module.exports.getAddressCoordinate = async (address) => {
    const url = `https://nominatim.openstreetmap.org/search?q=${encodeURIComponent(address)}&format=json`;
    const response = await axios.get(url, { headers: { 'User-Agent': 'QuickWheelsApp' } });
    if (response.data && response.data.length > 0) {
        const location = response.data[0];
        return { ltd: location.lat, lng: location.lon };
    } else {
        throw new Error('Unable to fetch coordinates');
    }
};

module.exports.getDistanceTime = async (origin, destination) => {
    if (!origin || !destination) {
        throw new Error('Origin and destination are required');
    }

    // origin and destination should be objects: { ltd, lng }
    const apiKey = process.env.ORS_API_KEY;
    const url = `https://api.openrouteservice.org/v2/directions/driving-car?api_key=${apiKey}&start=${origin.lng},${origin.ltd}&end=${destination.lng},${destination.ltd}`;

    try {
        const response = await axios.get(url);
        if (
            response.data &&
            response.data.features &&
            response.data.features.length > 0
        ) {
            const summary = response.data.features[0].properties.summary;
            return {
                distance: summary.distance, // in meters
                duration: summary.duration  // in seconds
            };
        } else {
            throw new Error('Unable to fetch distance and time');
        }
    } catch (err) {
        console.error(err);
        throw err;
    }
};

module.exports.getAutoCompleteSuggestions = async (input) => {
    const url = `https://nominatim.openstreetmap.org/search?q=${encodeURIComponent(input)}&format=json&addressdetails=1&limit=5`;
    const response = await axios.get(url, { headers: { 'User-Agent': 'QuickWheelsApp' } });
    if (response.data && response.data.length > 0) {
        // Return formatted address suggestions
        return response.data.map(item => item.display_name);
    } else {
        return [];
    }
};

module.exports.getCaptainsInTheRadius = async (ltd, lng, radius) => {

    // radius in km


    const captains = await captainModel.find({
        location: {
            $geoWithin: {
                $centerSphere: [ [ ltd, lng ], radius / 6371 ]
            }
        }
    });

    return captains;


}