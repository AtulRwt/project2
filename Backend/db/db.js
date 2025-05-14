const mongoose = require('mongoose');


function connectToDb() {
    // Using hardcoded connection string as a fallback if environment variable is not available
    const connectionString = process.env.DB_CONNECT || 'mongodb://localhost:27017/quick-wheels';
    
    mongoose.connect(connectionString
    ).then(() => {
        console.log('Connected to DB');
    }).catch(err => console.log(err));
}


module.exports = connectToDb;